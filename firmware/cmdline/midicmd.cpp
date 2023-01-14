// midiout.cpp
#include <iostream>
#include <cstdlib>

#include <RtMidi.h>

#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( milliseconds * 1000 )
#endif

int main()
{
  RtMidiOut *midiout = new RtMidiOut();
  std::vector<unsigned char> message;
  std::string name;
  int port;
  // Check available ports.
  unsigned int nPorts = midiout->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    goto cleanup;
  }
  std::cout << nPorts << " ports available!\n";
  for (port = 0; port < nPorts; port++) {
    name = midiout->getPortName(port);
    std::cout << "Found " << name << "\n";
    if (name.find("Vindor") != -1) {
      std::cout << "Open port " << name << "\n";
      midiout->openPort( port );
      message.push_back( 0xF0 );
      message.push_back( 0x00 );
      message.push_back( 0x20 );
      message.push_back( 0x50 );
      message.push_back( 0x17 );
      message.push_back( 0x53 );
      message.push_back( 0xF7 );
      midiout->sendMessage( &message );
      SLEEP(100);
      break;
    }
  }
  // Clean up
 cleanup:
  delete midiout;
  return 0;
}
