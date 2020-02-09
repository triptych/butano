#include "../include/btn_hw_audio.h"

#include "maxmod.h"
#include "btn_config_audio.h"
#include "../include/btn_hw_irq.h"

extern const uint8_t _btn_audio_soundbank_bin[];

namespace btn::hw::audio
{

namespace
{
    static_assert(BTN_CFG_AUDIO_MAX_CHANNELS > 0, "Invalid max audio channels");

    constexpr int _max_channels = BTN_CFG_AUDIO_MAX_CHANNELS;

    constexpr int _mix_length()
    {
        return BTN_CFG_AUDIO_KHZ == BTN_AUDIO_KHZ_8 ? MM_MIXLEN_8KHZ :
                BTN_CFG_AUDIO_KHZ == BTN_AUDIO_KHZ_10 ? MM_MIXLEN_10KHZ :
                BTN_CFG_AUDIO_KHZ == BTN_AUDIO_KHZ_13 ? MM_MIXLEN_13KHZ :
                BTN_CFG_AUDIO_KHZ == BTN_AUDIO_KHZ_16 ? MM_MIXLEN_16KHZ :
                BTN_CFG_AUDIO_KHZ == BTN_AUDIO_KHZ_18 ? MM_MIXLEN_18KHZ :
                BTN_CFG_AUDIO_KHZ == BTN_AUDIO_KHZ_21 ? MM_MIXLEN_21KHZ :
                BTN_CFG_AUDIO_KHZ == BTN_AUDIO_KHZ_27 ? MM_MIXLEN_27KHZ :
                MM_MIXLEN_31KHZ;
    }


    alignas(sizeof(int)) BTN_DATA_EWRAM uint8_t maxmod_engine_buffer[
            _max_channels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH + MM_SIZEOF_MIXCH) + _mix_length()];

    alignas(sizeof(int)) uint8_t maxmod_mixing_buffer[_mix_length()];

    volatile bool locked = false;


    class lock
    {

    public:
        lock()
        {
            while(locked)
            {
            }

            locked = true;
        }

        ~lock()
        {
            locked = false;
        }
    };


    void _commit_handler()
    {
        lock lock;

        mmFrame();
    }
}

void init()
{
    lock lock;

    mm_gba_system maxmod_info;
    maxmod_info.mixing_mode = mm_mixmode(BTN_CFG_AUDIO_KHZ);
    maxmod_info.mod_channel_count = _max_channels;
    maxmod_info.mix_channel_count = _max_channels;
    maxmod_info.module_channels = mm_addr(maxmod_engine_buffer);
    maxmod_info.active_channels = mm_addr(maxmod_engine_buffer + (_max_channels * MM_SIZEOF_MODCH));
    maxmod_info.mixing_channels = mm_addr(maxmod_engine_buffer +
            (_max_channels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH)));
    maxmod_info.mixing_memory = mm_addr(maxmod_mixing_buffer);
    maxmod_info.wave_memory = mm_addr(maxmod_engine_buffer +
            (_max_channels * (MM_SIZEOF_MODCH + MM_SIZEOF_ACTCH + MM_SIZEOF_MIXCH)));
    maxmod_info.soundbank = mm_addr(_btn_audio_soundbank_bin);
    mmInit(&maxmod_info);
}

void add_irq()
{
    lock lock;

    irq::add(irq::id::VBLANK, mmVBlank);
}

void remove_irq()
{
    lock lock;

    irq::remove(irq::id::VBLANK);
}

bool music_playing()
{
    return mmActive();
}

void play_music(int id, int volume, bool loop)
{
    mmStart(mm_word(id), loop ? MM_PLAY_LOOP : MM_PLAY_ONCE);
    mmSetModuleVolume(mm_word(volume));
}

void stop_music()
{
    mmStop();
}

void pause_music()
{
    mmPause();
}

void resume_music()
{
    mmResume();
}

void set_music_volume(int volume)
{
    mmSetModuleVolume(mm_word(volume));
}

void play_sound(int id)
{
    mmEffectRelease(mmEffect(mm_word(id)));
}

void play_sound(int id, int volume, int speed, int panning)
{
    mm_sound_effect sound_effect;
    sound_effect.id = mm_word(id);
    sound_effect.rate = mm_hword(speed);
    sound_effect.handle = 0;
    sound_effect.volume = mm_byte(volume);
    sound_effect.panning = mm_byte(panning);
    mmEffectRelease(mmEffectEx(&sound_effect));
}

void stop_all_sounds()
{
    mmEffectCancelAll();
}

int direct_sound_control_value()
{
    lock lock;

    return REG_SNDDSCNT;
}

void set_direct_sound_control_value(int value)
{
    lock lock;

    REG_SNDDSCNT = uint16_t(value);
}

void disable_vblank_handler()
{
    lock lock;

    mmSetVBlankHandler(nullptr);
}

void enable_vblank_handler()
{
    lock lock;

    mmFrame();
    mmSetVBlankHandler(reinterpret_cast<void*>(_commit_handler));
}

}
