#include "audio_engine_backend.hpp"

#include <cstdlib>
#include <dsound.h>
#include <windows.h>

#include <macros/macros.hpp>

namespace audio {

namespace {
#define DIRECT_SOUND_CREATE(name)                                                        \
    HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);
}   // namespace

class AudioEngineBackend::Impl {
  private:
    LPDIRECTSOUNDBUFFER fpPrimaryBuffer;
    LPDIRECTSOUNDBUFFER fpSecondaryBuffer;
    LPDIRECTSOUND fpDirectSound;

  public:
    // @todo move
    LPDIRECTSOUNDBUFFER primaryBuffer() { return fpPrimaryBuffer; }
    LPDIRECTSOUNDBUFFER secondaryBuffer() { return fpSecondaryBuffer; }
    LPDIRECTSOUND directSound() { return fpDirectSound; }

    Impl() {
        // NOTE(casey): Load the library
        HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
        if (DSoundLibrary) {
            // NOTE(casey): Get a DirectSound object! - cooperative
            direct_sound_create* DirectSoundCreate =
                (direct_sound_create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

            // TODO(casey): Double-check that this works on XP - DirectSound8 or 7??
            if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &fpDirectSound, 0))) {
                HWND window = GetForegroundWindow();
                if (window == NULL) { window = GetDesktopWindow(); }
                if (SUCCEEDED(
                        fpDirectSound->SetCooperativeLevel(window, DSSCL_PRIORITY))) {
                    DSBUFFERDESC BufferDescription = {};
                    BufferDescription.dwSize       = sizeof(BufferDescription);
                    BufferDescription.dwFlags      = DSBCAPS_PRIMARYBUFFER;

                    // NOTE(casey): "Create" a primary buffer
                    // TODO(casey): DSBCAPS_GLOBALFOCUS?
                    if (SUCCEEDED(fpDirectSound->CreateSoundBuffer(
                            &BufferDescription, &fpPrimaryBuffer, 0))) {
                    } else {
                        // TODO(casey): Diagnostic
                    }
                } else {
                    // TODO(casey): Diagnostic
                }

            } else {
                // TODO(casey): Diagnostic
            }
        } else {
            // TODO(casey): Diagnostic
        }
    }
};
}   // namespace audio
