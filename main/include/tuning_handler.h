#ifndef TURNING_HANDLER_H
#define TURNING_HANDLER_H

#include <stdint.h>
#include <strings.h>

// TODO: save database to nvm
// TODO: save last tuning to nvm

namespace AutoTuner
{
class TuningHandler
{
    public:
        static constexpr size_t MAX_NUMBER_OF_INSTRUMENTS = 6;
        static constexpr size_t MAX_NUMBER_OF_TUNINGS     = 8;
        static constexpr size_t MAX_NUMBER_OF_STRINGS     = 12;

        struct Tuning
        {
            const char *name;
            double     freq[MAX_NUMBER_OF_STRINGS];
            const char *notes[MAX_NUMBER_OF_STRINGS];
        };

        struct Instrument
        {
            const char *name;
            int        number_of_strings;
            int        number_of_tunings;
            Tuning     tunings[MAX_NUMBER_OF_TUNINGS];
        };

        struct Selection
        {
            int instrument;
            int tuning;
        };

    public:
        TuningHandler(const Selection &_selection={0, 0});
        bool setSelection(int instrument, int tuning);
        void updateFrequency(float frequency);
        void setCurrentString(int currentString);
        char * getTableCsv();
        char * getChoicesCsv();

    public:
        static const char       *TAG;
        static const Instrument instruments[MAX_NUMBER_OF_INSTRUMENTS];
        Selection               current_selection;
        float                   currentFrequency = 0;
        int                     currentString    = 0;
        char                    csvBuffer[1024]  = {0};
};
}

#endif // TURNING_HANDLER_H
