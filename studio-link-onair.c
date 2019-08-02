#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <re/re.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "baresip.h"

#define STUDIOLINKONAIR_URI "http://studio-link.de/plugins/lv2/studio-link-onair"

pthread_t tid;

void effectlive_src(const float* const input0, const float* const input1,
		unsigned long nframes);


typedef enum {
	AMP_INPUT0  = 0,
	AMP_INPUT1  = 1,
	AMP_OUTPUT0  = 2,
	AMP_OUTPUT1 = 3
} PortIndex;

typedef struct {
	// Port buffers
	const float* input0;
	const float* input1;
	float*       output0;
	float*       output1;
} Amp;

bool running = false;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{

	Amp* amp = (Amp*)malloc(sizeof(Amp));

	if (!running) {

		(void)re_fprintf(stderr, "activate baresip v%s"
				" Copyright (C) 2010 - 2015"
				" Alfred E. Heggestad et al.\n",
				BARESIP_VERSION);
		(void)sys_coredump_set(true);
		libre_init();
		conf_configure(false);
		baresip_init(conf_config());
		ua_init("baresip v" BARESIP_VERSION " (" ARCH "/" OS ")",
				true, true, true);
		conf_modules();
		pthread_create(&tid, NULL, (void*(*)(void*))&re_main, NULL);
		running = true;
	}

	return (LV2_Handle)amp;
}

static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	Amp* amp = (Amp*)instance;

	switch ((PortIndex)port) {
	case AMP_INPUT0:
		amp->input0 = (const float*)data;
		break;
	case AMP_INPUT1:
		amp->input1 = (const float*)data;
		break;
	case AMP_OUTPUT0:
		amp->output0 = (float*)data;
		break;
	case AMP_OUTPUT1:
		amp->output1 = (float*)data;
		break;
	}
}

static void
activate(LV2_Handle instance)
{
}

/**
   The `run()` method is the main process function of the plugin.  It processes
   a block of audio in the audio context.  Since this plugin is
   `lv2:hardRTCapable`, `run()` must be real-time safe, so blocking (e.g. with
   a mutex) or memory allocation are not allowed.
*/
static void
run(LV2_Handle instance, uint32_t n_samples)
{
	const Amp* amp = (const Amp*)instance;

	const float* const input0  = amp->input0;
	const float* const input1  = amp->input1;
	float* const       output0 = amp->output0;
	float* const       output1 = amp->output1;

	effectlive_src(input0, input1, n_samples);
}

static void
deactivate(LV2_Handle instance)
{
}

/**
   Destroy a plugin instance (counterpart to `instantiate()`).

   This method is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static void
cleanup(LV2_Handle instance)
{
	Amp* amp = (Amp*)instance;

	if (running) {
		//re_cancel();
		ua_stop_all(false);
		//(void)pthread_join(tid, NULL);
		sys_msleep(800);
		ua_close();
		re_cancel();
		conf_close();
		baresip_close();
		mod_close();
		libre_close();
		running = false;
	}

	free(instance);
}

static const void*
extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
	STUDIOLINKONAIR_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index) {
	case 0:  return &descriptor;
	default: return NULL;
	}
}
