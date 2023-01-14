/*
  ==============================================================================

    InfoTableModel.h
    Created: 27 Apr 2018 2:23:36pm
    Author:  Fernando Trias

  ==============================================================================
*/

#pragma once


class InfoTabModel : public TableListBoxModel
{
    StringArray labels;
    StringArray data;

public:
    int getNumRows() override
    {
        return labels.size();
    }

    void clear()
    {
        labels.clear();
        data.clear();
    }

    void addRow(const char *label, std::string value)
    {
        String sx = value;
        labels.add(label);
        data.add(sx);
    }

    void addRow(std::string &label, int value)
    {
        String sx = label;
        String sy(value);
        labels.add(sx);
        data.add(sy);
    }

    void addRow(const char *label, int value)
    {
        String sx(value);
        labels.add(label);
        data.add(sx);
    }

    void addRow(const char *label, String &value)
    {
        labels.add(label);
        data.add(value);
    }

    void addRow(String &label, String &value)
    {
        labels.add(label);
        data.add(value);
    }

    virtual void paintCell(Graphics & g	,
                           int 	rowNumber,
                           int 	columnId,
                           int 	width,
                           int 	height,
                           bool 	rowIsSelected
                           ) override
    {
        String sx;
        if (columnId==1) {
            sx = labels[rowNumber];
        }
        else{
            sx = data[rowNumber];
        }
        g.setColour(Colours::white);
        g.drawText(sx, 5, 0, width, height, Justification::centredLeft, true);
    }

    void paintRowBackground	(	Graphics & g	,
                             int 	rowNumber,
                             int 	width,
                             int 	height,
                             bool 	rowIsSelected
                             ) override
    {
    }
};
