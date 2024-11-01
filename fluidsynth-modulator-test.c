//#include <glib.h>
#include <stdlib.h> // malloc, free
#include <stdio.h> // printf
#include <string.h>
#include <strings.h>
#include <math.h>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
typedef float fluid_real_t;

/**
 * Flags defining the polarity, mapping function and type of a modulator source.
 * Compare with SoundFont 2.04 PDF section 8.2.
 *
 * Note: Bit values do not correspond to the SoundFont spec!  Also note that
 * #FLUID_MOD_GC and #FLUID_MOD_CC are in the flags field instead of the source field.
 */
enum fluid_mod_flags
{
    FLUID_MOD_POSITIVE = 0,       /**< Mapping function is positive */
    FLUID_MOD_NEGATIVE = 1,       /**< Mapping function is negative */
    FLUID_MOD_UNIPOLAR = 0,       /**< Mapping function is unipolar */
    FLUID_MOD_BIPOLAR = 2,        /**< Mapping function is bipolar */
    FLUID_MOD_LINEAR = 0,         /**< Linear mapping function */
    FLUID_MOD_CONCAVE = 4,        /**< Concave mapping function */
    FLUID_MOD_CONVEX = 8,         /**< Convex mapping function */
    FLUID_MOD_SWITCH = 12,        /**< Switch (on/off) mapping function */
    FLUID_MOD_GC = 0,             /**< General controller source type (#fluid_mod_src) */
    FLUID_MOD_CC = 16,             /**< MIDI CC controller (source will be a MIDI CC number) */

    FLUID_MOD_SIN = 0x80,            /**< Custom non-standard sinus mapping function */
};

#define FLUID_VEL_CB_SIZE 128
#define FLUID_PEAK_ATTENUATION  960.0f
//#define M_LN10 2.3025850929940456840179914546844

static double fluid_concave_tab[FLUID_VEL_CB_SIZE];
static double fluid_convex_tab[FLUID_VEL_CB_SIZE];

static void fluid_conversion_config(void){
	int i;
	long double x;

	/* initialize the conversion tables (see fluid_mod.c
		fluid_mod_get_value cases 4 and 8) */

	/* concave unipolar positive transform curve */
	fluid_concave_tab[0] = 0.0;
	fluid_concave_tab[FLUID_VEL_CB_SIZE - 1] = 1.0;

	/* convex unipolar positive transform curve */
	fluid_convex_tab[0] = 0;
	fluid_convex_tab[FLUID_VEL_CB_SIZE - 1] = 1.0;

	/* There seems to be an error in the specs. The equations are
		implemented according to the pictures on SF2.01 page 73. */

	for(i = 1; i < FLUID_VEL_CB_SIZE - 1; i++)
	{
			x = (-200.0L * 2 / FLUID_PEAK_ATTENUATION) * logl(i / (FLUID_VEL_CB_SIZE - 1.0L)) / M_LN10;
			fluid_convex_tab[i] = (1.0L - x);
			fluid_concave_tab[(FLUID_VEL_CB_SIZE - 1) - i] =  x;
	}
}


/*
 * fluid_concave
 */
fluid_real_t
fluid_concave(fluid_real_t val)
{
	printf("fluid_concave val: %f\n", val);
    int ival = (int)val;
		printf("ival: %d\n", ival);
    if(val < 0.f)
    {
			printf("val < 0.f: true\n");
        return 0.f;
    }
    else if (ival >= FLUID_VEL_CB_SIZE - 1)
    {
			printf("ival >= FLUID_VEL_CB_SIZE - 1: true\n");
			printf("fluid_concave_tab[FLUID_VEL_CB_SIZE - 1]: %lf\n", fluid_concave_tab[FLUID_VEL_CB_SIZE - 1]);
        return fluid_concave_tab[FLUID_VEL_CB_SIZE - 1];
    }
printf("fluid_concave_tab[ival]: %lf\n", fluid_concave_tab[ival]);
printf("fluid_concave_tab[ival + 1]: %lf\n", fluid_concave_tab[ival + 1]);
printf("fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]: %lf\n", fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]);
printf("val - ival: %f\n", val - ival);
printf("(fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]) * (val - ival): %lf\n", (fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]) * (val - ival));
printf("fluid_concave_tab[ival] + (fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]) * (val - ival): %%lf %lf\n", (fluid_concave_tab[ival] + (fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]) * (val - ival)));
printf("fluid_concave_tab[ival] + (fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]) * (val - ival): %%f %f\n", fluid_concave_tab[ival] + (fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]) * (val - ival));
    return fluid_concave_tab[ival] + (fluid_concave_tab[ival + 1] - fluid_concave_tab[ival]) * (val - ival);
}

/*
 * fluid_convex
 */
fluid_real_t
fluid_convex(fluid_real_t val)
{
	printf("fluid_convex val: %f\n", val);
    int ival = (int)val;
		printf("ival: %d\n", ival);
    if(val < 0.f)
    {
			printf("val < 0.f: true\n");
        return 0.f;
    }
    else if (ival >= FLUID_VEL_CB_SIZE - 1)
    {
			printf("ival >= FLUID_VEL_CB_SIZE - 1: true\n");
			printf("fluid_convex_tab[FLUID_VEL_CB_SIZE - 1]: %lf\n", fluid_convex_tab[FLUID_VEL_CB_SIZE - 1]);
        return fluid_convex_tab[FLUID_VEL_CB_SIZE - 1];
    }
printf("fluid_convex_tab[ival]: %lf\n", fluid_convex_tab[ival]);
printf("fluid_convex_tab[ival + 1]: %lf\n", fluid_convex_tab[ival + 1]);
printf("fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]: %lf\n", fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]);
printf("val - ival: %f\n", val - ival);
printf("(fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]) * (val - ival): %lf\n", (fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]) * (val - ival));
printf("fluid_convex_tab[ival] + (fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]) * (val - ival): %%lf %lf\n", fluid_convex_tab[ival] + (fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]) * (val - ival));
printf("fluid_convex_tab[ival] + (fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]) * (val - ival): %%f %f\n", fluid_convex_tab[ival] + (fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]) * (val - ival));
    // interpolation between convex steps: fixes bad sounds with modenv and filter cutoff
    return fluid_convex_tab[ival] + (fluid_convex_tab[ival + 1] - fluid_convex_tab[ival]) * (val - ival);
}

/*
 * retrieves the initial value from the given source of the modulator
 */
//static fluid_real_t
//fluid_mod_get_source_value(const unsigned char mod_src,
//                           const unsigned char mod_flags,
//                           fluid_real_t *range,
//                           const fluid_voice_t *voice
//                          )
//{
//    const fluid_channel_t *chan = voice->channel;
//    fluid_real_t val;
//
//    if(mod_flags & FLUID_MOD_CC)
//    {
//        val = fluid_channel_get_cc(chan, mod_src);
//
//        if(mod_src == PORTAMENTO_CTRL)
//        {
//            // an invalid portamento fromkey should be treated as 0 when it's actually used for moulating
//            if(!fluid_channel_is_valid_note(val))
//            {
//                val = 0;
//            }
//        }
//    }
//    else
//    {
//        switch(mod_src)
//        {
//        case FLUID_MOD_NONE:         /* SF 2.01 8.2.1 item 0: src enum=0 => value is 1 */
//            val = *range;
//            break;
//
//        case FLUID_MOD_VELOCITY:
//            val = fluid_voice_get_actual_velocity(voice);
//            break;
//
//        case FLUID_MOD_KEY:
//            val = fluid_voice_get_actual_key(voice);
//            break;
//
//        case FLUID_MOD_KEYPRESSURE:
//            val = fluid_channel_get_key_pressure(chan, voice->key);
//            break;
//
//        case FLUID_MOD_CHANNELPRESSURE:
//            val = fluid_channel_get_channel_pressure(chan);
//            break;
//
//        case FLUID_MOD_PITCHWHEEL:
//            val = fluid_channel_get_pitch_bend(chan);
//            *range = 0x4000;
//            break;
//
//        case FLUID_MOD_PITCHWHEELSENS:
//            val = fluid_channel_get_pitch_wheel_sensitivity(chan);
//            break;
//
//        default:
//            FLUID_LOG(FLUID_ERR, "Unknown modulator source '%d', disabling modulator.", mod_src);
//            val = 0.0;
//        }
//    }
//
//    return val;
//}

/**
 * transforms the initial value retrieved by \c fluid_mod_get_source_value into [0.0;1.0]
 */
static fluid_real_t
fluid_mod_transform_source_value(fluid_real_t val, unsigned char mod_flags, const fluid_real_t range)
{
    /* normalized value, i.e. usually in the range [0;1] */
    const fluid_real_t val_norm = val / range;
		printf("val / range: %f / %f\n", val, range);
		printf("val_norm: %f\n", val_norm);

    /* we could also only switch case the lower nibble of mod_flags, however
     * this would keep us from adding further mod types in the future
     *
     * instead just remove the flag(s) we already took care of
     */
		printf("mod_flags: %i\n", mod_flags);
		printf("FLUID_MOD_CC: %d\n", FLUID_MOD_CC);
    mod_flags &= ~FLUID_MOD_CC;
		printf("mod_flags: %i\n", mod_flags);

    switch(mod_flags/* & 0x0f*/)
    {
    case FLUID_MOD_LINEAR | FLUID_MOD_UNIPOLAR | FLUID_MOD_POSITIVE: /* =0 */
        val = val_norm;
        break;

    case FLUID_MOD_LINEAR | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE: /* =1 */
        val = 1.0f - val_norm;
        break;

    case FLUID_MOD_LINEAR | FLUID_MOD_BIPOLAR | FLUID_MOD_POSITIVE: /* =2 */
        val = -1.0f + 2.0f * val_norm;
        break;

    case FLUID_MOD_LINEAR | FLUID_MOD_BIPOLAR | FLUID_MOD_NEGATIVE: /* =3 */
        val = 1.0f - 2.0f * val_norm;
        break;

    case FLUID_MOD_CONCAVE | FLUID_MOD_UNIPOLAR | FLUID_MOD_POSITIVE: /* =4 */
        val = fluid_concave(127 * (val_norm));
        break;

    case FLUID_MOD_CONCAVE | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE: /* =5 */
				printf("1.0f - val_norm: %f\n", 1.0f - val_norm);
				printf("127 * (1.0f - val_norm): %f\n", 127 * (1.0f - val_norm));
        val = fluid_concave(127 * (1.0f - val_norm)); // IMPORTANT
        break;

    case FLUID_MOD_CONCAVE | FLUID_MOD_BIPOLAR | FLUID_MOD_POSITIVE: /* =6 */
        val = (val_norm > 0.5f) ?  fluid_concave(127 * 2 * (val_norm - 0.5f))
              : -fluid_concave(127 * 2 * (0.5f - val_norm));
        break;

    case FLUID_MOD_CONCAVE | FLUID_MOD_BIPOLAR | FLUID_MOD_NEGATIVE: /* =7 */
        val = (val_norm > 0.5f) ? -fluid_concave(127 * 2 * (val_norm - 0.5f))
              :  fluid_concave(127 * 2 * (0.5f - val_norm));
        break;

    case FLUID_MOD_CONVEX | FLUID_MOD_UNIPOLAR | FLUID_MOD_POSITIVE: /* =8 */
				printf("127 * (val_norm): %f\n", 127 * (val_norm));
        val = fluid_convex(127 * (val_norm)); // IMPORTANT
        break;

    case FLUID_MOD_CONVEX | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE: /* =9 */
        val = fluid_convex(127 * (1.0f - val_norm));
        break;

    case FLUID_MOD_CONVEX | FLUID_MOD_BIPOLAR | FLUID_MOD_POSITIVE: /* =10 */
        val = (val_norm > 0.5f) ?  fluid_convex(127 * 2 * (val_norm - 0.5f))
              : -fluid_convex(127 * 2 * (0.5f - val_norm));
        break;

    case FLUID_MOD_CONVEX | FLUID_MOD_BIPOLAR | FLUID_MOD_NEGATIVE: /* =11 */
        val = (val_norm > 0.5f) ? -fluid_convex(127 * 2 * (val_norm - 0.5f))
              :  fluid_convex(127 * 2 * (0.5f - val_norm));
        break;

    case FLUID_MOD_SWITCH | FLUID_MOD_UNIPOLAR | FLUID_MOD_POSITIVE: /* =12 */
        val = (val_norm >= 0.5f) ? 1.0f : 0.0f;
        break;

    case FLUID_MOD_SWITCH | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE: /* =13 */
        val = (val_norm >= 0.5f) ? 0.0f : 1.0f;
        break;

    case FLUID_MOD_SWITCH | FLUID_MOD_BIPOLAR | FLUID_MOD_POSITIVE: /* =14 */
        val = (val_norm >= 0.5f) ? 1.0f : -1.0f;
        break;

    case FLUID_MOD_SWITCH | FLUID_MOD_BIPOLAR | FLUID_MOD_NEGATIVE: /* =15 */
        val = (val_norm >= 0.5f) ? -1.0f : 1.0f;
        break;

    /*
     * MIDI CCs only have a resolution of 7 bits. The closer val_norm gets to 1,
     * the less will be the resulting change of the sinus. When using this sin()
     * for scaling the cutoff frequency, there will be no audible difference between
     * MIDI CCs 118 to 127. To avoid this waste of CCs multiply with 0.87
     * (at least for unipolar) which makes sin() never get to 1.0 but to 0.98 which
     * is close enough.
     */
    //case FLUID_MOD_SIN | FLUID_MOD_UNIPOLAR | FLUID_MOD_POSITIVE: /* custom sin(x) */
    //    val = FLUID_SIN((FLUID_M_PI / 2.0f * 0.87f) * val_norm);
    //    break;
    //
    //case FLUID_MOD_SIN | FLUID_MOD_UNIPOLAR | FLUID_MOD_NEGATIVE: /* custom */
    //    val = FLUID_SIN((FLUID_M_PI / 2.0f * 0.87f) * (1.0f - val_norm));
    //    break;
    //
    //case FLUID_MOD_SIN | FLUID_MOD_BIPOLAR | FLUID_MOD_POSITIVE: /* custom */
    //    val = (val_norm > 0.5f) ?  FLUID_SIN(FLUID_M_PI * (val_norm - 0.5f))
    //          : -FLUID_SIN(FLUID_M_PI * (0.5f - val_norm));
    //    break;
    //
    //case FLUID_MOD_SIN | FLUID_MOD_BIPOLAR | FLUID_MOD_NEGATIVE: /* custom */
    //    val = (val_norm > 0.5f) ? -FLUID_SIN(FLUID_M_PI * (val_norm - 0.5f))
    //          :  FLUID_SIN(FLUID_M_PI * (0.5f - val_norm));
    //    break;

    default:
        //FLUID_LOG(FLUID_ERR, "Unknown modulator type '%d', disabling modulator.", mod_flags);
				fprintf(stderr, "Unknown modulator type '%d', disabling modulator.\n", mod_flags);
        val = 0.0f;
        break;
    }

    return val;
}

/*
 * fluid_mod_get_value.
 * Computes and return modulator output following SF2.01
 * (See SoundFont Modulator Controller Model Chapter 9.5).
 *
 * Output = Transform(Amount * Map(primary source input) * Map(secondary source input))
 *
 * Notes:
 * 1)fluid_mod_get_value, ignores the Transform operator. The result is:
 *
 *   Output = Amount * Map(primary source input) * Map(secondary source input)
 *
 * 2)When primary source input (src1) is set to General Controller 'No Controller',
 *   output is forced to 0.
 *
 * 3)When secondary source input (src2) is set to General Controller 'No Controller',
 *   output is forced to +1.0 
 */
//fluid_real_t
//fluid_mod_get_value(fluid_mod_t *mod, fluid_voice_t *voice)
//{
//    extern fluid_mod_t default_vel2filter_mod;
//
//    fluid_real_t v1 = 0.0, v2 = 1.0;
//    fluid_real_t final_value;
//    /* The wording of the default modulators refers to a range of 127/128.
//     * And the table in section 9.5.3 suggests, that this mapping should be applied
//     * to all unipolar and bipolar mappings respectively.
//     *
//     * Thinking about this further, this is actually pretty clever, as this is properly
//     * addresses MIDI Recommended Practice (RP-036) Default Pan Formula
//     * "Since MIDI controller values range from 0 to 127, the exact center
//     * of the range, 63.5, cannot be represented."
//     *
//     * When changing the overall range to 127/128 however, the "middle pan" value of 64
//     * can be correctly represented.
//     */
//    fluid_real_t range1 = 128.0, range2 = 128.0;
//
//    /* 'special treatment' for default controller
//     *
//     *  Reference: SF2.01 section 8.4.2
//     *
//     * The GM default controller 'vel-to-filter cut off' is not clearly
//     * defined: If implemented according to the specs, the filter
//     * frequency jumps between vel=63 and vel=64.  To maintain
//     * compatibility with existing sound fonts, the implementation is
//     * 'hardcoded', it is impossible to implement using only one
//     * modulator otherwise.
//     *
//     * I assume here, that the 'intention' of the paragraph is one
//     * octave (1200 cents) filter frequency shift between vel=127 and
//     * vel=64.  'amount' is (-2400), at least as long as the controller
//     * is set to default.
//     *
//     * Further, the 'appearance' of the modulator (source enumerator,
//     * destination enumerator, flags etc) is different from that
//     * described in section 8.4.2, but it matches the definition used in
//     * several SF2.1 sound fonts (where it is used only to turn it off).
//     * */
//    if(fluid_mod_test_identity(mod, &default_vel2filter_mod))
//    {
//// S. Christian Collins' mod, to stop forcing velocity based filtering
//        /*
//            if (voice->vel < 64){
//              return (fluid_real_t) mod->amount / 2.0;
//            } else {
//              return (fluid_real_t) mod->amount * (127 - voice->vel) / 127;
//            }
//        */
//        return 0; // (fluid_real_t) mod->amount / 2.0;
//    }
//
//// end S. Christian Collins' mod
//
//    /* get the initial value of the first source */
//    if(mod->src1 > 0)
//    {
//        v1 = fluid_mod_get_source_value(mod->src1, mod->flags1, &range1, voice);
//
//        /* transform the input value */
//        v1 = fluid_mod_transform_source_value(v1, mod->flags1, range1);
//    }
//    /* When primary source input (src1) is set to General Controller 'No Controller',
//       output is forced to 0.0
//    */
//    else
//    {
//        return 0.0;
//    }
//
//    /* no need to go further */
//    if(v1 == 0.0f)
//    {
//        return 0.0f;
//    }
//
//    /* get the second input source */
//    if(mod->src2 > 0)
//    {
//        v2 = fluid_mod_get_source_value(mod->src2, mod->flags2, &range2, voice);
//
//        /* transform the second input value */
//        v2 = fluid_mod_transform_source_value(v2, mod->flags2, range2);
//    }
//    /* When secondary source input (src2) is set to General Controller 'No Controller',
//       output is forced to +1.0
//    */
//    else
//    {
//        v2 = 1.0f;
//    }
//
//    /* it indeed is as simple as that: */
//    final_value = (fluid_real_t) mod->amount * v1 * v2;
//
//    /* check for absolute value transform */
//    if(mod->trans == FLUID_MOD_TRANSFORM_ABS)
//    {
//        final_value = FLUID_FABS(final_value);
//    }
//    return final_value;
//}

#ifdef EMSCRIPTEN
EMSCRIPTEN_KEEPALIVE
#endif
fluid_real_t myConvert(int inNum /*value of the cc, 0-127*/, int modFlags, int multiplier){
	fluid_conversion_config();
	
	fluid_real_t v1 = 0.0, v2 = 1.0;
    fluid_real_t final_value;
    /* The wording of the default modulators refers to a range of 127/128.
     * And the table in section 9.5.3 suggests, that this mapping should be applied
     * to all unipolar and bipolar mappings respectively.
     *
     * Thinking about this further, this is actually pretty clever, as this is properly
     * addresses MIDI Recommended Practice (RP-036) Default Pan Formula
     * "Since MIDI controller values range from 0 to 127, the exact center
     * of the range, 63.5, cannot be represented."
     *
     * When changing the overall range to 127/128 however, the "middle pan" value of 64
     * can be correctly represented.
     */
    fluid_real_t range1 = 128.0, range2 = 128.0;
		
	v1 = inNum;
	v1 = fluid_mod_transform_source_value(v1, modFlags, range1);
	printf("v1 after fluid_mod_transform_source_value: %f\n", v1);
	final_value = (fluid_real_t) multiplier/*amount*/ * v1 * v2;
	printf("final_value: %f\n", final_value);
	//printf("final_value - 10000: %f\n", final_value - 10000);
	return final_value;
}

int main(int argc, char *argv[]){
	myConvert(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
}