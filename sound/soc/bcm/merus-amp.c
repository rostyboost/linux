/*
 * ASoC Driver for Merus Audio Raspberry Pi HAT Sound Card
 *
 * Author:	Jorgen Ariel Muszkat <ariel.muszkat@infineon.com>
 *		Copyright 2018
 *    based on code by Jorgen Kragh Jakobsen <jkj@merus-audio.com>
 *		based on code by Daniel Matuschek <info@crazy-audio.com>
 *		based on code by Florian Meier <florian.meier@koalo.de>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>

#include "../codecs/ma120x0p.h"

static int snd_rpi_merus_amp_init(struct snd_soc_pcm_runtime *rtd)
{
  printk(KERN_INFO "Init....\n");
	return 0;
}

static int snd_rpi_merus_amp_hw_params(struct snd_pcm_substream *substream,
				       struct snd_pcm_hw_params *params)
{
  //int ret2;

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	return snd_soc_dai_set_bclk_ratio(cpu_dai, 64);
  /*
	unsigned int sample_bits =
		snd_pcm_format_physical_width(params_format(params));
	ret2 = snd_soc_dai_set_bclk_ratio(cpu_dai, sample_bits * 2);
  printk(KERN_INFO "set blck ratio returned: %d\n", sample_bits * 2);
  return ret2;
  */
}

/* machine stream operations */
static struct snd_soc_ops snd_rpi_merus_amp_ops = {
	.hw_params = snd_rpi_merus_amp_hw_params,
};

static struct snd_soc_dai_link snd_rpi_merus_amp_dai[] = {
{
	.name		= "MerusAmp" ,
	.stream_name	= "Merus Audio Amp",
	.cpu_dai_name	= "bcm2708-i2s.0",
	.codec_dai_name	= "ma120x0p-amp",
	.platform_name	= "bcm2708-i2s.0",
	.codec_name	= "ma120x0p.1-0020",
	.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
				SND_SOC_DAIFMT_CBS_CFS,
	.ops		= &snd_rpi_merus_amp_ops,
	.init		= snd_rpi_merus_amp_init,
},
};

/* audio machine driver */
static struct snd_soc_card snd_rpi_merus_amp = {
	.name         = "snd_rpi_merus_amp",
	.driver_name  = "MerusAudioAmp",
	.owner        = THIS_MODULE,
	.dai_link     = snd_rpi_merus_amp_dai,
	.num_links    = ARRAY_SIZE(snd_rpi_merus_amp_dai),
};

static int snd_rpi_merus_amp_probe(struct platform_device *pdev)
{
	int ret = 0;
  printk(KERN_INFO "debug probe stage 1\n" );
	snd_rpi_merus_amp.dev = &pdev->dev;

	if (pdev->dev.of_node) {

			struct device_node *i2s_node;
	    struct snd_soc_dai_link *dai = &snd_rpi_merus_amp_dai[0];
			printk(KERN_INFO "in pdev...\n" );

	    i2s_node = of_parse_phandle(pdev->dev.of_node,
					"i2s-controller", 0);

	    if (i2s_node) {
      	printk(KERN_INFO "in i2s_node...\n" );
        dai->cpu_dai_name = NULL;
			  dai->cpu_of_node = i2s_node;
		   	dai->platform_name = NULL;
		  	dai->platform_of_node = i2s_node;
	    }
	}
	printk(KERN_INFO "debug probe stage 2 \n" );

	ret = snd_soc_register_card(&snd_rpi_merus_amp);
	if (ret && ret != -EPROBE_DEFER)
		dev_err(&pdev->dev,
			"snd_soc_register_card() failed: %d\n", ret);

	printk(KERN_INFO "debug probe all good exit\n");

	return ret;
};

static int snd_rpi_merus_amp_remove(struct platform_device *pdev)
{
  printk(KERN_INFO "snd_rpi_merus_amp exit\n");

	return snd_soc_unregister_card(&snd_rpi_merus_amp);
};


static const struct of_device_id snd_rpi_merus_amp_of_match[] = {
	{ .compatible = "merus,merus-amp", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_rpi_merus_amp_of_match);


static struct platform_driver snd_rpi_merus_amp_driver = {
	.driver = {
		.name   = "snd-rpi-merus-amp",
		.owner  = THIS_MODULE,
		.of_match_table = snd_rpi_merus_amp_of_match,
	},
	.probe          = snd_rpi_merus_amp_probe,
	.remove         = snd_rpi_merus_amp_remove,
};

module_platform_driver(snd_rpi_merus_amp_driver);


MODULE_AUTHOR("Ariel Muszkat <ariel.muszkat@infineon.com>");
MODULE_DESCRIPTION("ASoC Driver for Merus Audio Amp HAT Sound Card");
MODULE_LICENSE("GPL v2");
