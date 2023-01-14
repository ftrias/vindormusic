/**
 * Copyright (c) 20011-2017 Bill Greiman
 * This file is part of the SdFs library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef ExFatVolume_h
#define ExFatVolume_h
#include "ExFatPartition.h"
#include "ExFatFile.h"
//=============================================================================
/**
 * \class ExFatVolume
 * \brief exFAT volume.
 */
class ExFatVolume : public ExFatPartition {
 public:
  ExFatVolume() {
  }
  /**
   * Initialize an FatVolume object.
   * \param[in] dev Device block driver.
   * \param[in] setCwv Set current working volume if true.
   * \param[in] part partition to initialize.
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */
  bool begin(BlockDevice* dev, bool setCwv = true, uint8_t part = 0) {
    if (!(part ? init(dev, part) : init(dev, 1) || init(dev, 0))) {
      return false;
    }
    if (!chdir()) {
      return false;
    }
    if (setCwv) {
      m_cwv = this;
    }
    return true;
  }
  /**
   * Set volume working directory to root.
   * \return true for success else false.
   */
  bool chdir() {
    m_vwd.close();
    return m_vwd.openRoot(this);
  }
  /**
   * Set volume working directory.
   * \param[in] path Path for volume working directory.
   * \return true for success or false for failure.
   */
  bool chdir(const char *path);
  /** \return current working volume. */
  static ExFatVolume* cwv() {return m_cwv;}
  /** Change global working volume to this volume. */
  void chvol() {m_cwv = this;}

  /**
   * Test for the existence of a file.
   *
   * \param[in] path Path of the file to be tested for.
   *
   * \return true if the file exists else false.
   */
  bool exists(const ExChar_t* path) {
    ExFatFile tmp;
    return tmp.open(this, path, O_READ);
  }

#if ENABLE_ARDUINO_FEATURES

  /** List the directory contents of the root directory to Serial.
   *
   */
  void ls() {
    ls(&Serial);
  }
   /** List the directory contents of the volume root to Serial.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   */
  void ls(uint8_t flags) {
    ls(&Serial, flags);
  }
  /** List the directory contents of a directory to Serial.
   *
   * \param[in] path directory to list.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   */
  void ls(const ExChar_t* path, uint8_t flags = 0) {
    ls(&Serial, path, flags);
  }
  /** open a file
   *
   * \param[in] path location of file to be opened.
   * \param[in] mode open mode flags.
   * \return a File object.
   */
  ExFile open(const ExChar_t* path, uint8_t mode = O_READ) {
    ExFile tmpFile;
    tmpFile.open(this, path, mode);
    return tmpFile;
  }
  /** open a file
   *
   * \param[in] path location of file to be opened.
   * \param[in] mode open mode flags.
   * \return a File object.
   */
  ExFile open(const String &path, uint8_t mode = O_READ) {
    return open(path.c_str(), mode );
  }
#endif  // ENABLE_ARDUINO_FEATURES
  //----------------------------------------------------------------------------
  /** List the directory contents of the root directory.
   *
   * \param[in] pr Print stream for list.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   */
  void ls(print_t* pr, uint8_t flags = 0) {
    m_vwd.ls(pr, flags);
  }
  /** List the directory contents of a directory.
   *
   * \param[in] pr Print stream for list.
   *
   * \param[in] path directory to list.
   *
   * \param[in] flags The inclusive OR of
   *
   * LS_DATE - %Print file modification date
   *
   * LS_SIZE - %Print file size.
   *
   * LS_R - Recursive list of subdirectories.
   */
  void ls(print_t* pr, const ExChar_t* path, uint8_t flags) {
    ExFatFile dir;
    dir.open(this, path, O_READ);
    dir.ls(pr, flags);
  }
  /** Make a subdirectory in the volume root directory.
   *
   * \param[in] path A path with a valid 8.3 DOS name for the subdirectory.
   *
   * \param[in] pFlag Create missing parent directories if true.
   *
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */
  bool mkdir(const ExChar_t* path, bool pFlag = true) {
    ExFatFile sub;
    return sub.mkdir(vwd(), path, pFlag);
  }

  /** Remove a file from the volume root directory.
   *
   * \param[in] path A path with a valid 8.3 DOS name for the file.
   *
   * \return The value true is returned for success and
   * the value false is returned for failure.
  */
  bool remove(const ExChar_t* path) {
    ExFatFile tmp;
    return tmp.open(this, path, O_WRITE) && tmp.remove();
  }
  /** Rename a file or subdirectory.
   *
   * \param[in] oldPath Path name to the file or subdirectory to be renamed.
   *
   * \param[in] newPath New path name of the file or subdirectory.
   *
   * The \a newPath object must not exist before the rename call.
   *
   * The file to be renamed must not be open.  The directory entry may be
   * moved and file system corruption could occur if the file is accessed by
   * a file object that was opened before the rename() call.
   *
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */
  bool rename(const ExChar_t* oldPath, const ExChar_t* newPath) {
    ExFatFile file;
    return file.open(vwd(), oldPath, O_READ) && file.rename(vwd(), newPath);
  }
  /** Remove a subdirectory from the volume's working directory.
   *
   * \param[in] path A path with a valid 8.3 DOS name for the subdirectory.
   *
   * The subdirectory file will be removed only if it is empty.
   *
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */
  bool rmdir(const ExChar_t* path) {
    ExFatFile sub;
    return sub.open(this, path, O_READ) && sub.rmdir();
  }
  /** Truncate a file to a specified length.  The current file position
   * will be at the new EOF.
   *
   * \param[in] path A path with a valid 8.3 DOS name for the file.
   * \param[in] length The desired length for the file.
   *
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */
  bool truncate(const ExChar_t* path, uint64_t length) {
    ExFatFile file;
    if (!file.open(this, path, O_WRITE)) {
      return false;
    }
    return file.truncate(length);
  }
  //============================================================================
#if  USE_UNICODE_NAMES
  // Not implemented when Unicode is selected.
  bool exists(const char* path);
  bool mkdir(const char* path, bool pFlag = true);
  bool remove(const char* path);
  bool rename(const char* oldPath, const char* newPath);
  bool rmdir(const char* path);
#endif  //  USE_UNICODE_NAMES

 private:
  friend ExFatFile;
  ExFatFile* vwd() {return &m_vwd;}
  ExFatFile m_vwd;
  static ExFatVolume* m_cwv;
};
#endif  // ExFatVolume_h
