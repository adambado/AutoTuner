#include "tuning_handler.h"

#include "led_handler.h"

#include "esp_log.h"

#include <stdint.h>
#include <strings.h>
#include <string.h>

namespace AutoTuner
{
const char                      *TuningHandler::TAG                                   = "TuningHandler";

const TuningHandler::Instrument TuningHandler::instruments[MAX_NUMBER_OF_INSTRUMENTS] =
{
    {
        .name              = "6string-guitar",
        .number_of_strings = 6,
        .number_of_tunings = 8,
        .tunings           = {            // Guitar 6 string standard tuning
            {
                .name      = "Standard",
                .freq      =
                {
                    82.41, 110.00, 146.83, 196.00, 246.94, 329.63
                },
                .notes     =
                {
                    "E", "A", "D", "G", "B", "E"
                }
            },
            { // Guitar 6 string Drop D
                .name      = "Drop D",
                .freq      =
                {
                    73.42, 110.00, 146.83, 196.00, 246.94, 329.63
                },
                .notes     =
                {
                    "D", "A", "D", "G", "B", "E"
                }
            },
            { // Guitar 6 string Drop C
                .name      = "Drop C",
                .freq      =
                {
                    65.41, 98.00, 130.81, 174.61, 220.00, 293.66
                },
                .notes     =
                {
                    "C", "G", "C", "F", "A", "D"
                }
            },
            { // Guitar 6 sring DADGAD
                .name      = "DADGAD",
                .freq      =
                {
                    73.42, 110, 146.83, 196.00, 220.00, 293.67
                },
                .notes     =
                {
                    "D", "A", "D", "G", "A", "D"
                }
            },
            { // Guitar 6 sring Open A
                .name      = "Open A",
                .freq      =
                {
                    82.41, 110.00, 138.60, 164.80, 220.0, 329.6
                },
                .notes     =
                {
                    "E", "A", "C#", "E", "A", "E"
                }
            },
            { // Guitar 6 sring Open D
                .name      = "Open D",
                .freq      =
                {
                    73.42, 110.00, 146.83, 185.00, 220.00, 293.67
                },
                .notes     =
                {
                    "D", "A", "D", "F#", "A", "D"
                }
            },
            { // Guitar 6 string open G
                .name      = "Open G",
                .freq      =
                {
                    73.42, 98.00, 146.83, 196.00, 246.94, 293.67
                },
                .notes     =
                {
                    "D", "G", "D", "G", "B", "D"
                }
            },
            {
                .name      = "Half step down",
                .freq      =
                {
                    73.42, 103.80, 138.60, 185.00, 233.10, 311.4
                },
                .notes     =
                {
                    "D#", "G#", "C#", "F#", "A#", "D#"
                }
            }
        }
    },
    {
        .name              = "7 string guitar",
        .number_of_strings = 7,
        .number_of_tunings = 1,
        .tunings           = {
            { // Standard tuning
                .name      = "Standard",
                .freq      =
                {
                    61.74, 82.41, 110.00, 146.83, 196.00, 246.94, 329.63
                },
                .notes     =
                {
                    "B", "E", "A", "D", "G", "B", "E"
                }
            }
        }
    },
    {
        .name              = "12 string guitar",
        .number_of_strings = 12,
        .number_of_tunings = 1,
        .tunings           = {
            { // Standard tuning
                .name      = "Standard",
                .freq      =
                {
                    164.8, 82.41, 220.0, 110.0, 293.7, 146.8, 392.0, 196.0, 493.9, 493.9, 329.6, 329.6
                },
                .notes     =
                {
                    "E", "E", "A", "A", "D", "G", "G", "B", "B", "E", "E"
                }
            }
        }
    },
    {
        .name              = "4string-ukulele",
        .number_of_strings = 4,
        .number_of_tunings = 5,
        .tunings           = {
            { // Ukulele 4 string C standard tuning
                .name      = "C standard",
                .freq      =
                {
                    392.00, 293.66, 220.00, 146.83
                },
                .notes     =
                {
                    "G", "C", "E", "A"
                }
            },
            { // Ukulele 4 string D standard tuning
                .name      = "D standard",
                .freq      =
                {
                    440.00, 293.70, 370.0, 493.9
                },
                .notes     =
                {
                    "A", "D", "F#", "B"
                }
            },
            { // Ukulele 4 string tenor C tuning
                .name      = "Tenor C",
                .freq      =
                {
                    196.00, 261.60, 329.6, 440.0
                },
                .notes     =
                {
                    "G", "C", "E", "A"
                }
            },
            { // Ukulele 4 string open C tuning
                .name      = "Open C",
                .freq      =
                {
                    196.00, 261.60, 329.6, 392.00
                },
                .notes     =
                {
                    "G", "C", "E", "G"
                }
            },
            { // All fifths
                .name      = "All fifths",
                .freq      =
                {
                    196.00, 293.70, 440.00, 329.6
                },
                .notes     =
                {
                    "G", "D", "A", "E"
                }
            }
        }
    },
    {
        .name              = "4 string bass",
        .number_of_strings = 4,
        .number_of_tunings = 2,
        .tunings           = {
            { // 4 string bass standard
                .name      = "Standard",
                .freq      =
                {
                    41.20, 55, 73.42, 98.00
                },
                .notes     =
                {
                    "E", "A", "D", "G"
                }
            },
            { // 4 string bass drop D
                .name      = "Drop D",
                .freq      =
                {
                    36.71, 55.00, 73.42, 98.00
                },
                .notes     =
                {
                    "D", "A", "D", "G"
                }
            }
        }
    },
    {
        .name              = "5 string bass",
        .number_of_strings = 5,
        .number_of_tunings = 1,
        .tunings           = {
            { // Ukulele 4 string C standard tuning
                .name      = "C standard",
                .freq      =
                {
                    30.87, 41.20, 55.00, 73.42, 98.00
                },
                .notes     =
                {
                    "B", "E", "A", "D", "G"
                }
            }
        }
    }
};

TuningHandler::TuningHandler(const Selection &_selection) :
    current_selection{_selection}
{
    ESP_LOGI(TAG, "Constructor");
}

bool TuningHandler::setSelection(int instrument, int tuning)
{
    if (instrument < 0 || instrument >= MAX_NUMBER_OF_INSTRUMENTS)
    {
        return false;
    }

    if (tuning < 0 || tuning >= instruments[instrument].number_of_tunings)
    {
        return false;
    }

    current_selection.instrument = instrument;
    current_selection.tuning     = tuning;

    return true;
}

void TuningHandler::setCurrentString(int _currentString)
{
    currentString = _currentString;
}

void TuningHandler::updateFrequency(float frequency)
{
    currentFrequency = frequency;
}

char * TuningHandler::getTableCsv()
{
    csvBuffer[0] = '\0';
    currentFrequency++;
    strcat(csvBuffer, "string;current;goal;note\n");

    char lineBuffer[256] = {0};

    for (int i = 0; i < instruments[current_selection.instrument].number_of_strings; ++i)
    {
        int        stringNumber  = i + 1;
        double     goalFrequency = instruments[current_selection.instrument].tunings[current_selection.tuning].freq[i];
        const char *note         = instruments[current_selection.instrument].tunings[current_selection.tuning].notes[i];

        sprintf(lineBuffer, "%d;%.2f;%.2f;%s\n", stringNumber, currentFrequency, goalFrequency, note);
        strcat(csvBuffer, lineBuffer);
    }

    sprintf(lineBuffer, "%d\n", currentString);
    strcat(csvBuffer, lineBuffer);

    return csvBuffer;
}

char * TuningHandler::getChoicesCsv()
{
    csvBuffer[0] = '\0';
    // Add the header to the CSV buffer
    strcat(csvBuffer, "ivalue;itext;tvalue;ttext\n");

    // Temporary buffer to hold each line
    char lineBuffer[256];

    // Iterate through the instruments and tunings to append the values to csvBuffer
    for (int i = 0; i < MAX_NUMBER_OF_INSTRUMENTS; ++i)
    {
        for (int j = 0; j < MAX_NUMBER_OF_TUNINGS; ++j)
        {
            if (instruments[i].name == NULL || instruments[i].tunings[j].name == NULL)
            {
                break;
            }

            int        instrumentId    = i;
            const char *instrumentName = instruments[i].name;
            int        tuningId        = j;
            const char *tuningName     = instruments[i].tunings[j].name;

            // Format the line and append it to csvBuffer
            sprintf(lineBuffer, "%d;%s;%d;%s\n", instrumentId, instrumentName, tuningId, tuningName);
            strcat(csvBuffer, lineBuffer);
        }
    }

    return csvBuffer;
}
}
