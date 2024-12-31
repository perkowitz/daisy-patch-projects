/**
 * Copy of libdaisy wavplayer class allowing samples to be accessed at any index.
 * Doesn't inherit from wavplayer because the buffer is private.
 * 
*/
#ifndef INDEXABLEWAVPLAYER_H
#define INDEXABLEWAVPLAYER_H

#include "daisy_patch.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

#define WAV_FILENAME_MAX \
    256 /**< Maximum LFN (set to same in FatFs (ffconf.h) */


class IndexableWavPlayer {
  public:
    IndexableWavPlayer() {}
    ~IndexableWavPlayer() {}

    /** Initializes the WavPlayer, loading up to max_files of wav files from an SD Card. */
    void Init(const char* search_path);

    /** Opens the file at index sel for reading.
    \param sel File to open
     */
    int Open(size_t sel);

    /** Closes whatever file is currently open.
    \return &
     */
    int Close();

    /** \return The next sample if playing, otherwise returns 0 */
    int16_t Stream();

    /** Collects buffer for playback when needed. */
    void Prepare();

    /** Resets the playback position to the beginning of the file immediately */
    void Restart();

    /** Sets whether or not the current file will repeat after completing playback. 
    \param loop To loop or not to loop.
    */
    inline void SetLooping(bool loop) { looping_ = loop; }

    /** \return Whether the WavPlayer is looping or not. */
    inline bool GetLooping() const { return looping_; }

    /** \return The number of files loaded by the WavPlayer */
    inline size_t GetNumberFiles() const { return file_cnt_; }

    /** \return currently selected file.*/
    inline size_t GetCurrentFile() const { return file_sel_; }

  private:
    enum BufferState
    {
        BUFFER_STATE_IDLE,
        BUFFER_STATE_PREPARE_0,
        BUFFER_STATE_PREPARE_1,
    };

    BufferState GetNextBuffState();

    static constexpr size_t kMaxFiles   = 8;
    static constexpr size_t kBufferSize = 4096;
    WavFileInfo             file_info_[kMaxFiles];
    size_t                  file_cnt_, file_sel_;
    BufferState             buff_state_;
    int16_t                 buff_[kBufferSize];
    size_t                  read_ptr_;
    bool                    looping_, playing_;
    FIL                     fil_;
};

} // namespace daisy

#endif
