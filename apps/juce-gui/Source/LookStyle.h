/*
  ==============================================================================

    LookStyle.h
    Created: 2 Apr 2018 11:29:24am
    Author:  Fernando Trias

  ==============================================================================
*/

#pragma once

class KnobLookAndFeel : public LookAndFeel_V4
{
    public:
    Image knob;
    
    KnobLookAndFeel() {
    }
    
    void drawRotarySlider (Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &slider)
    {
        const int sz = knob.getWidth();
        const int num = knob.getHeight() / sz - 1;
        const int frame = (int)(sliderPosProportional * num);

        int dim = width<height?width:height;

        x += (width - dim) / 2;

        g.drawImage(knob, x, y, dim, dim, 0, frame * sz, sz, sz);
    }
};

#define KNOB(CLASS,PNG) \
class CLASS ## KnobLookAndFeel : public KnobLookAndFeel \
{ \
public: \
    CLASS ## KnobLookAndFeel() { \
        knob = ImageCache::getFromMemory (PNG, PNG ## Size); \
    } \
};

KNOB(Cutoff, BinaryData::knob_cutoff_png);
KNOB(LFOFreq, BinaryData::knob_lfo_freq_png);
KNOB(Resonance, BinaryData::knob_resonance_png);
KNOB(LFODelay, BinaryData::knob_delay_png);

class WaveSlideLookAndFeel : public KnobLookAndFeel
{
public:
    WaveSlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_wave_png, BinaryData::knob_wave_pngSize);
    }
};

class HarmonicSlideLookAndFeel : public KnobLookAndFeel
{
public:
    HarmonicSlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_octave_png, BinaryData::knob_octave_pngSize);
    }
};

class ChorusSlideLookAndFeel : public KnobLookAndFeel
{
public:
    ChorusSlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_chorus_png, BinaryData::knob_chorus_pngSize);
    }
};

class KnobSlideLookAndFeel : public KnobLookAndFeel
{
public:
    KnobSlideLookAndFeel(const char *png, int size) {
        knob = ImageCache::getFromMemory (png, size);
    }
};

class DetuneSlideLookAndFeel : public KnobLookAndFeel
{
public:
    DetuneSlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_detune_png, BinaryData::knob_detune_pngSize);
    }
};

class BipolarSlideLookAndFeel : public KnobLookAndFeel
{
public:
    BipolarSlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_bipolar_png, BinaryData::knob_bipolar_pngSize);
    }
};

class UnipolarSlideLookAndFeel : public KnobLookAndFeel
{
public:
    UnipolarSlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_unipolar_png, BinaryData::knob_unipolar_pngSize);
    }
};

class SlideLookAndFeel : public LookAndFeel_V4
{
    public:
    Image knob;
    int w, h;
    SlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::slider1_png, BinaryData::slider1_pngSize);
        w = 64;
        h = 128;
    }
    virtual void drawLinearSlider (Graphics& g,
                                   int x, int y,
                                   int width, int height,
                                   float sliderPos,
                                   float minSliderPos,
                                   float maxSliderPos,
                                   const Slider::SliderStyle style,
                                   Slider& slider)
    {
//        const int w = 32;
//        const int h = 128;
        const int num = knob.getWidth() / w - 1;
        double val = slider.getValue();
        double max = slider.getMaximum();
        double min = slider.getMinimum();
        double prop = (val - min) / (max - min);
        const int frame = (int)(prop * num);

//        const int mwidth = height/4;
        const int mwidth = width;
        x += (width - mwidth) / 2;

        g.drawImage(knob, x, y, mwidth, height, frame * w, 0, w, h);
    }
};

class HorizontalSlideLookAndFeel : public LookAndFeel_V4
{
public:
    Image knob;
    HorizontalSlideLookAndFeel() {
        knob = ImageCache::getFromMemory (BinaryData::knob_unipolar_png, BinaryData::knob_unipolar_pngSize);
    }
    virtual void drawLinearSlider (Graphics& g,
                                   int x, int y,
                                   int width, int height,
                                   float sliderPos,
                                   float minSliderPos,
                                   float maxSliderPos,
                                   const Slider::SliderStyle style,
                                   Slider& slider)
    {
        const int w = 128;
        const int h = 32;
        const int num = knob.getHeight() / h - 1;
        double val = slider.getValue();
        double max = slider.getMaximum();
        double min = slider.getMinimum();
        double prop = (val - min) / (max - min);
        const int frame = (int)(prop * num);

        const int mheight = width/4;

        y += (mheight - mheight) / 2;

        g.drawImage(knob, x, y, width, mheight, 0, frame * h, w, h);
    }
};

class TabLookAndFeel: public LookAndFeel_V4
{
    public:
    TabLookAndFeel() 
    {
//        setColour(TabbedComponent::ColourIds::outlineColourId, Colours::white);
    }
    virtual void drawTabButton (TabBarButton& tab, Graphics&  g, bool isMouseOver, bool isMouseDown)
    {
        String text = tab.getButtonText();

        int x2 = tab.getWidth();
        int y2 = tab.getHeight();
        
        /*
        int border = 4;
        g.drawLine(border, border, x2-border, border);
        g.drawLine(border, border, border, y2);
        g.drawLine(x2-border, border, x2-border, y2);
        */
        
        int border = 1;
        Colour fg = findColour(TabbedComponent::ColourIds::outlineColourId);
        g.setColour(fg);
        g.fillRoundedRectangle(0, 0, (float)x2, y2+20.0F, x2/6.0F);

        Colour bg = findColour(ResizableWindow::backgroundColourId);
        
        g.setColour(bg);
        g.fillRoundedRectangle(border, border, x2-2.0F*border, y2+20.0F, x2/6.0F);
        
        int sel = tab.isFrontTab();
        if (isMouseOver) {
            g.setColour(Colours::white);
        }
        else if (sel) {
            g.setColour(Colours::white);
        }
        else {
            g.setColour(Colours::lightgrey);
        }
        g.drawText(text, 0, 0, tab.getWidth(), tab.getHeight(), Justification::Flags::verticallyCentred | Justification::Flags::horizontallyCentred);
    }
};

class MidiInstrumentLookAndFeel : public LookAndFeel_V4 {
    void drawLabel(Graphics &g, Label &label)
    {
        g.fillAll (label.findColour (Label::backgroundColourId));
        
        if (! label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const Font font (getLabelFont (label));
            
            g.setColour (label.findColour (Label::textColourId).withMultipliedAlpha (alpha));
            g.setFont (font);
            
            Rectangle<int> textArea (label.getBorderSize().subtractedFrom (label.getLocalBounds()));
            
            String text = label.getText();
            int space = text.indexOfChar(' ');
            text = text.substring(0, space);
            
            g.drawFittedText (text, textArea, label.getJustificationType(),
                              jmax (1, (int) (textArea.getHeight() / font.getHeight())),
                              label.getMinimumHorizontalScale());
            
            g.setColour (label.findColour (Label::outlineColourId).withMultipliedAlpha (alpha));
        }
        else if (label.isEnabled())
        {
            g.setColour (label.findColour (Label::outlineColourId));
        }
        
        g.drawRect (label.getLocalBounds());
    }
};
