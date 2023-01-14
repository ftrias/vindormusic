#include "firmware.h"

#ifdef USE_SDCARD

#include <AudioSynthSDCard.h>
#include "vdebug.h"

// #include <AudioEffectTransform.h>
// extern AudioEffectDeclick declick;

#include "Display.h"

struct __attribute__((__packed__)) WavHeader {
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t Format;
    uint32_t Subchunk1ID;
    uint32_t Subchunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    uint32_t Subchunk2ID;
    uint32_t Subchunk2Size;
};

int SDCardFileBuffer::readHeader(File &pfile) {
    char p = pfile.peek();
    if (p!='R') { // rudimentary test for RIFF header
        return 0;
    }
    struct WavHeader head;
    pfile.read(&head, sizeof(head));

    // is this really a RIFF/WAV file?
    if (head.ChunkID != 0x46464952) {
        pfile.seek(0);
        return 0;
    }

    // check to make sure it's right format
    if (head.AudioFormat == 1 && head.NumChannels == 1 && head.SampleRate == 44100 && head.BitsPerSample == 16) {
        // ok
    }
    else {
        showDebug("WAV file invalid format");
        return 0;
    }

    return sizeof(head);
}

bool SDCardFileBuffer::open(File &pfile, const char *filename, int loopStart, int loopEnd) {
    pfile.close();
    pfile = SD.open(filename);
    if (!pfile) {
        showDebug("Open failed:", filename);
        return false;
    }

    file_offset = readHeader(pfile);

    loop_start = file_offset/2 + loopStart;
    if (loopEnd) {
        loop_end = file_offset/2 + loopEnd;
    }
    else {
        loop_end = 0;
    }
    readindex = 0;
    writeindex = -1;
    buffer_count = 0;

    return true;
}

void SDCardFileBuffer::startLoop(File &pfile) {
    file_offset = loop_start*2;
    pfile.seek(file_offset);
}

int SDCardFileBuffer::readBlockRaw(File &pfile, int16_t* data, int read_len) {
    if (readbusy) {
        return -1;
    }
    readbusy = 1;
    
    int n, bytes, bytesread=0;
        
    // if we are going to read into the loop end, loop back
    if (loop_end && file_offset + read_len >= loop_end*2) {
        // bytesread = 0;
        while(read_len > 0 && pfile.available()) {
            // how much space do we have for reading?
            bytes = loop_end*2 - file_offset;
            if (bytes > read_len) bytes = read_len; // plenty, so read it all
            n = pfile.read((uint8_t*)data + bytesread, bytes);
            if (n<1) { // some sort of problem
                readbusy = 0;
                return -1;
            }
            
            bytesread += n;
            read_len -= n;
            file_offset += n;          
            
            // go back to beginning of loop and read remaining items
            if (file_offset >= loop_end*2) {
                startLoop(pfile);
            }
        }
        readbusy = 0;
        return bytesread;
    }
    else { // not looping OR plenty of data left, so read a whole block
        n = pfile.read((uint8_t*)data, read_len);
        if (n<=0) { // end of file or other error
            // blank out data
            for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
                data[i] = 0;
            }
            readbusy = 0;
            return -1;
        }
        file_offset += n;
        bytesread += n;
        
        // if not enough data, fill remaining block with 0s
        if (n < AUDIO_BLOCK_SAMPLES*2) {
            for (int i=n/2; i < AUDIO_BLOCK_SAMPLES; i++) {
                data[i] = 0;
            }
        }
    }
    readbusy = 0;
    return bytesread;
}

int16_t *SDCardFileBuffer::getNextBlock() {
    // if (buffer_count <= 0) { // no more blocks
    //     return NULL;
    // }
    int16_t *r = readbuffer[readindex];
    if (++readindex >= readcount) readindex = 0;
    buffer_count--;
    return r;
}

int16_t *SDCardFileBuffer::getNextBlock(File &pfile, int peek) {
    // if (buffer_count <= 0) { // no more blocks
    //     return NULL;
    // }
    int16_t *r = readbuffer[readindex];
    if (!peek) {
        if (++readindex >= readcount) readindex = 0;   
        buffer_count--;
    }
    return r;
}

void SDCardFileBuffer::readBlockFill(File &pfile) {
    for (int i=0; i<readcount; i++) {
        if (writeindex == readindex) break;
        if (writeindex == -1) writeindex = 0; // first time, load it all
        int16_t *r = readbuffer[writeindex];
        int ret = readBlockRaw(pfile, r, AUDIO_BLOCK_SAMPLES*2);
        if (ret > 0) {
            buffer_count++;
            if (++writeindex >= readcount) writeindex = 0;
        }
    }
}

/////////////

// int crosscurve[crosssamples];

void AudioSynthSDCard::begin()
{
}

void AudioSynthSDCard::setFactor(int f)
{   
}

bool AudioSynthSDCard::play(const char *filename, int loopStart, int loopEnd, int jumpToLoop)
{
    skipblocks = 0;
    pskip = pcurrent;
    skipupdate = 1;
    if (playing) {
        pcross = pcurrent;
        pcurrent = pcurrent?0:1;
        crossfade = crossblocks;
        crossidx = 0; 
        cushionmax = AUDIO_BLOCK_SAMPLES * 2;
        // declick.setStrength(6);
    }
    else {
        crossfade = 0;
    }
    pc[pcurrent].open(pfile, filename, loopStart, loopEnd);
    if (crossfade || jumpToLoop) pc[pcurrent].startLoop(pfile);
    pc[pcurrent].readBlockFill(pfile);
    stop_flag = 0;
    playing = 1;
    skipupdate = 0;
#ifdef USE_SD_SPEED
    play_speed = -257; // start new file
#endif
    return true;
}

void AudioSynthSDCard::readahead()
{
    pc[pcurrent].readBlockFill(pfile); 
}

bool AudioSynthSDCard::isPlaying() 
{
    return playing; 
}

void AudioSynthSDCard::update()
{
    if (playing==0) return;

#ifdef USE_SD_SPEED
    if (play_speed) {
        update_speed();
        return;
    }
#endif

    int16_t *b;
    
    if (skipupdate) {
        b = pc[pskip].getNextBlock();
        if (b == NULL) return;
    }
    else {
        b = pc[pcurrent].getNextBlock();
        if (b == NULL) return;
        
        if (crossfade) {
            crossfade--;
            int16_t *px = pc[pcross].getNextBlock();
            if (px) {
                for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
                    int val = px[i] * (crosssamples - crossidx) / crosssamples;
                    val += b[i] * crossidx / crosssamples;
                    b[i] = val;
                    crossidx++;
                    if (crossidx >= crosssamples-1) {
                        crossfade = 0;
                        // declick.setStrength();
                        break;
                    }
                }
            }
        }
    }
    
    audio_block_t *block;
    // allocate the audio blocks to transmit
    block = allocate();
    if (block == NULL) {
        showDebug("out of memory");
        return;
    }
    
    for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
        magnitude.update(desired_magnitude);  
        block->data[i] = ((b[i] * magnitude) >> 15);
    }
    
    transmit(block);
    release(block);
    
    if (magnitude==0 && desired_magnitude==0 && stop_flag) {
        playing=0;
    }
}

#ifdef USE_SD_SPEED

void AudioSynthSDCard::update_speed()
{
    // if (playing==0) return;
    
    audio_block_t *block;
    // allocate the audio blocks to transmit
    block = allocate();
    if (block == NULL) {
        debugOutput("out of memory");
        return;
    }
    
    static int16_t *b = NULL;
    static int bidx = 0;
    const int bidx_bits = 8;
    const int bidx_step = (1 << bidx_bits);
    const int bidx_mask = (1 << bidx_bits) - 1;
    const int bidx_max = AUDIO_BLOCK_SAMPLES << bidx_bits;
    
    int pi = skipupdate ? pskip : pcurrent;

    if (play_speed < 0) {
        bidx = play_speed; // will become 0 later
        b = pc[pi].getNextBlock(pfile);
        play_speed = -play_speed;
    }

    for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
        bidx += play_speed;
        if (bidx >= bidx_max) {
            bidx -= bidx_max;
            b = pc[pi].getNextBlock(pfile);
        }

        #if 0
        int v = b[bidx >> bidx_bits];
        #else
        int v;
        int bi = bidx >> bidx_bits;
        int bi2 = bi+1;
        int bix = bidx & bidx_mask;
        if (bi2 >= AUDIO_BLOCK_SAMPLES) {  // going over boundry
            int16_t *b2 = pc[pi].getNextBlock(pfile, 1); // peek next block
            v = LinearInterpolateDiv(b[bi], b2[0], bix, bidx_step);
        }
        else {
            v = LinearInterpolateDiv(b[bi], b[bi2], bix, bidx_step);
        }
        #endif

        magnitude = cushion2(magnitude, desired_magnitude, 10);
        block->data[i] = ((v * magnitude) >> 16);
    }
    
    transmit(block);
    release(block);
    
    if (magnitude==0 && desired_magnitude==0 && stop_flag) {
        playing=0;
    }
}

#endif

void AudioSynthSDCard::amplitude(float n)
{
    desired_magnitude = n * 32767.0f;
    if (n==0.0f) stop_flag = 1;
}

void AudioSynthSDCard::stop()
{
    desired_magnitude = 0;
    playing = 0;
    stop_flag = 1;
}

#endif
