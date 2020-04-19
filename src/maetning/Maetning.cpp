/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

#define PARAM_SATURATION 0
#define PARAM_TYPE 1
#define PARAM_MASTERVOLUME 2
#define PARAM_MASTERMIX 3

#define NUM_PARAMS 4
#define NUM_SATURATIONS 4
#include "sat0.h"
#include "sat1.h"
#include "sat2.h"
#include "sat3.h"

// -----------------------------------------------------------------------------------------------------------

/**
  Plugin that demonstrates the latency API in DPF.
 */
class MaetningPlugin : public Plugin
{
public:
    MaetningPlugin() : Plugin(NUM_PARAMS, 0, 0) // 1st argument: Number of parameters
    {
        sampleRateChanged(getSampleRate());
    }

    ~MaetningPlugin() override
    {
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

   /**
      Get the plugin label.
      This label is a short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    */
    const char* getLabel() const override
    {
        return "maetning";
    }

   /**
      Get an extensive comment/description about the plugin.
    */
    const char* getDescription() const override
    {
        return "Saturation plugin";
    }

   /**
      Get the plugin author/maker.
    */
    const char* getMaker() const override
    {
        return "soerenbnoergaard";
    }

   /**
      Get the plugin homepage.
    */
    const char* getHomePage() const override
    {
        return "https://github.com/soerenbnoergaard/maetning";
    }

   /**
      Get the plugin license name (a single line of text).
      For commercial plugins this should return some short copyright information.
    */
    const char* getLicense() const override
    {
        return "MIT";
    }

   /**
      Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override
    {
        return d_version(0, 0, 0);
    }

   /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override
    {
        /* soerenbnoergaard: I just made something up */
        return d_cconst('e', 'K', 'A', 'p');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

   /**
      Initialize the parameter @a index.
      This function will be called once, shortly after the plugin is created.
    */
    void initParameter(uint32_t index, Parameter& parameter) override
    {

        switch (index) {
        case PARAM_SATURATION:
            parameter.hints  = kParameterIsAutomable | kParameterIsInteger;
            parameter.name   = "Saturation";
            parameter.symbol = "Saturation";
            parameter.unit   = "%";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        case PARAM_TYPE:
            parameter.hints  = kParameterIsAutomable | kParameterIsInteger;
            parameter.name   = "Type";
            parameter.symbol = "Type";
            parameter.unit   = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f * NUM_SATURATIONS - 1.0f;
            break;

        case PARAM_MASTERVOLUME:
            parameter.hints  = kParameterIsAutomable | kParameterIsInteger;
            parameter.name   = "MasterVolume";
            parameter.symbol = "MasterVolume";
            parameter.unit   = "dB";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = -51.0f;
            parameter.ranges.max = 0.0f;
            break;

        case PARAM_MASTERMIX:
            parameter.hints  = kParameterIsAutomable | kParameterIsInteger;
            parameter.name   = "MasterMix";
            parameter.symbol = "MasterMix";
            parameter.unit   = "%";
            parameter.ranges.def = 100.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 100.0f;
            break;

        default:
            break;
        }

        // Set the default parameter values
        setParameterValue(index, parameter.ranges.def);
    }

   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

   /**
      Get the current value of a parameter.
      The host may call this function from any context, including realtime processing.
    */
    float getParameterValue(uint32_t index) const override
    {
        switch (index) {
        case PARAM_SATURATION:
            return param_saturation;
            break;

        case PARAM_TYPE:
            return param_type;
            break;

        case PARAM_MASTERVOLUME:
            return param_mastervolume;
            break;

        case PARAM_MASTERMIX:
            return param_mastermix;
            break;

        default:
            return 0.0;
            break;
        }
    }

   /**
      Change a parameter value.
      The host may call this function from any context, including realtime processing.
      When a parameter is marked as automable, you must ensure no non-realtime operations are performed.
      @note This function will only be called for parameter inputs.
    */
    void setParameterValue(uint32_t index, float value) override
    {
        switch (index) {
        case PARAM_SATURATION:
            param_saturation = value;
            break;

        case PARAM_TYPE:
            param_type = value;
            param_type_int = (int)value;
            break;

        case PARAM_MASTERVOLUME:
            param_mastervolume = value;
            if (value < -50) {
                param_mastervolume_lin = 0.0;
            }
            else {
                param_mastervolume_lin = pow(10.0, value/20.0);
            }
            break;

        case PARAM_MASTERMIX:
            param_mastermix = value;
            param_mastermix_wet = value/100.0;
            param_mastermix_dry = 1.0 - param_mastermix_wet;
            break;

        default:
            break;
        }
    }

   /* --------------------------------------------------------------------------------------------------------
    * Audio/MIDI Processing */

   /**
      Run/process function for plugins without MIDI input.
      @note Some parameters might be null if there are no audio inputs or outputs.
    */
    void run(const float** inputs, float** outputs, uint32_t frames) override
    {
        float x = 0.0;
        float y = 0.0;
        float s = 0.0;

        float p0 = 0.0;
        float p1 = 0.0;
        float p2 = 0.0;
        float p3 = 0.0;
        float p4 = 0.0;
        float bp = 0.0;
        float bn = 0.0;

        switch (param_type_int) {
        case 0:
            p0 = sat0_coeffs[(int)param_saturation][0];
            p1 = sat0_coeffs[(int)param_saturation][1];
            p2 = sat0_coeffs[(int)param_saturation][2];
            p3 = sat0_coeffs[(int)param_saturation][3];
            p4 = sat0_coeffs[(int)param_saturation][4];
            bp = p2 - p1*p2/p0;
            bn = p4 - p3*p4/p0;
            break;

        case 1:
            p0 = sat1_coeffs[(int)param_saturation][0];
            p1 = sat1_coeffs[(int)param_saturation][1];
            p2 = sat1_coeffs[(int)param_saturation][2];
            break;

        case 2:
            p0 = sat2_coeffs[(int)param_saturation][0];
            p1 = sat2_coeffs[(int)param_saturation][1];
            p2 = sat2_coeffs[(int)param_saturation][2];
            p3 = sat2_coeffs[(int)param_saturation][3];
            break;

        case 3:
            p0 = sat3_coeffs[(int)param_saturation][0];
            p1 = sat3_coeffs[(int)param_saturation][1];
            p2 = sat3_coeffs[(int)param_saturation][2];
            break;

        };

        for (uint32_t ch = 0; ch < 2; ch++) {
            for (uint32_t n = 0; n < frames; n++) {
                x = inputs[ch][n];
                y = 0;

                // Apply saturation
                switch (param_type_int) {
                case 0:
                    y = p0*x;
                    if (y > p2) {
                        y = p1*x + bp;
                    }
                    else if (y < p4) {
                        y = p3*x + bn;
                    }
                    break;

                case 1:
                    y = x / (p1 + p0*std::abs(x)) + p2*std::abs(x);
                    break;

                case 2:
                    s = p0 * x;
                    if (s < -0.6) {
                        y = p1*s*s + p2*s + p3;
                    }
                    else if (s > 0.6) {
                        y = (-p1)*s*s + p2*s + (-p3);
                    }
                    else {
                        y = s;
                    }
                    break;

                case 3:
                    s = p0*x;
                    if (s < -0.75) {
                        y = p1*s + p2;
                    }
                    else if (s > 0.75) {
                        y = p1*s - p2;
                    }
                    else {
                        y = s;
                    }
                    break;

                default:
                    y = x;
                }

                // Mix wet and dry signal
                y = param_mastermix_wet*y + param_mastermix_dry*x;

                // Apply master volume
                y *= param_mastervolume_lin;

                // Write to output
                y = outputs[ch][n] = y;
            }
        }
    }

   /* --------------------------------------------------------------------------------------------------------
    * Callbacks (optional) */

   /**
      Optional callback to inform the plugin about a sample rate change.
      This function will only be called when the plugin is deactivated.
    */
    void sampleRateChanged(double newSampleRate) override
    {
    }

    // -------------------------------------------------------------------------------------------------------

private:

    float param_saturation;
    float param_type;
    int param_type_int;
    float param_mastervolume;
    float param_mastervolume_lin;
    float param_mastermix;
    float param_mastermix_wet;
    float param_mastermix_dry;

   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaetningPlugin)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new MaetningPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
