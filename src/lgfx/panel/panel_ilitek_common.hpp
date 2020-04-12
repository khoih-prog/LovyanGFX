#ifndef LGFX_PANEL_ILITEK_COMMON_HPP_
#define LGFX_PANEL_ILITEK_COMMON_HPP_

#include "panel_common.hpp"

namespace lgfx
{
  struct PanelIlitekCommon : public PanelCommon
  {
    PanelIlitekCommon() {
      cmd_caset  = CommandCommon::CASET;
      cmd_raset  = CommandCommon::RASET;
      cmd_ramwr  = CommandCommon::RAMWR;
      cmd_ramrd  = CommandCommon::RAMRD;
      cmd_rddid  = CommandCommon::RDDID;
      cmd_slpin  = CommandCommon::SLPIN;
      cmd_slpout = CommandCommon::SLPOUT;

      read_depth = rgb888_3Byte;
      len_dummy_read_pixel = 8;
      len_dummy_read_rddid = 1;
      len_setwindow = 32;
    }

  protected:

    const uint8_t* getInvertDisplayCommands(uint8_t* buf, bool invert) override
    {
      this->invert = invert;
      buf[2] = buf[0] = invert ? CommandCommon::INVON : CommandCommon::INVOFF;
      buf[3] = buf[1] = 0;
      buf[5] = buf[4] = 0xFF;
      return buf;
    }

    const uint8_t* getRotationCommands(uint8_t* buf, int_fast8_t r) override
    {
      rotation = r & 7;
      buf[0] = CommandCommon::MADCTL;
      buf[1] = 1;
      buf[2] = getMadCtl(rotation) | (rgb_order ? MAD_RGB : MAD_BGR);
      buf[3] = buf[4] = 0xFF;
      return buf;
    }

    virtual uint8_t getMadCtl(uint8_t r) const {
      static constexpr uint8_t madctl_table[] = {
                                         0,
        MAD_MV|MAD_MX|MAD_MH              ,
               MAD_MX|MAD_MH|MAD_MY|MAD_ML,
        MAD_MV|              MAD_MY|MAD_ML,
                             MAD_MY|MAD_ML,
        MAD_MV                            ,
               MAD_MX|MAD_MH              ,
        MAD_MV|MAD_MX|MAD_MY|MAD_MH|MAD_ML,
      };
      r = (((r + offset_rotation) & 3) | (r & 4));
      return madctl_table[r];
    }

    enum mad_t
    { MAD_MY  = 0x80
    , MAD_MX  = 0x40
    , MAD_MV  = 0x20
    , MAD_ML  = 0x10
    , MAD_BGR = 0x08
    , MAD_MH  = 0x04
    , MAD_RGB = 0x00
    };

    const uint8_t* getColorDepthCommands(uint8_t* buf, color_depth_t depth) override
    {
      PanelCommon::getColorDepthCommands(buf, depth);
      buf[0] = CommandCommon::COLMOD;
      buf[1] = 1;
      buf[2] = getColMod(write_depth);
      buf[3] = buf[4] = 0xFF;
      return buf;
    }

    struct CommandCommon {
    static constexpr uint8_t NOP     = 0x00;
    static constexpr uint8_t SWRESET = 0x01;
    static constexpr uint8_t RDDID   = 0x04;
    static constexpr uint8_t RDDST   = 0x09;
    static constexpr uint8_t SLPIN   = 0x10;
    static constexpr uint8_t SLPOUT  = 0x11;
    static constexpr uint8_t PTLON   = 0x12;
    static constexpr uint8_t NORON   = 0x13;
    static constexpr uint8_t INVOFF  = 0x20;
    static constexpr uint8_t INVON   = 0x21;
    static constexpr uint8_t GAMMASET= 0x26;
    static constexpr uint8_t DISPOFF = 0x28;
    static constexpr uint8_t DISPON  = 0x29;
    static constexpr uint8_t CASET   = 0x2A;
    static constexpr uint8_t RASET   = 0x2B; static constexpr uint8_t PASET = 0x2B;
    static constexpr uint8_t RAMWR   = 0x2C;
    static constexpr uint8_t RAMRD   = 0x2E;
    static constexpr uint8_t MADCTL  = 0x36;
    static constexpr uint8_t COLMOD  = 0x3A; static constexpr uint8_t PIXSET = 0x3A;
    };

    enum colmod_t
    { RGB565_2BYTE = 0x55
    , RGB666_3BYTE = 0x66
    };
    virtual uint8_t getColMod(uint8_t bpp) const { return (bpp > 16) ? RGB666_3BYTE : RGB565_2BYTE; }
  };
}

#endif
