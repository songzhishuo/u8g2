/*

  u8x8_d_uc1628.c

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2017, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

  UC1628: Monochrome 163x256 driver

  CAD: 011

*/

static const uint8_t u8x8_d_uc1628_powersave0_seq[] = {
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8X8_CA( 0x0c9, 0x0ad ),				/* display enable */  
  U8X8_END_TRANSFER(),             	/* disable chip */
  U8X8_END()             			/* end of sequence */
};

static const uint8_t u8x8_d_uc1628_powersave1_seq[] = {
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8X8_CA( 0x0c9, 0x0ac ),				/* display disable */  
  U8X8_END_TRANSFER(),             	/* disable chip */
  U8X8_END()             			/* end of sequence */
  U8X8_END()             			/* end of sequence */
};


static const uint8_t u8x8_d_uc1628_flip0_seq[] = {
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8X8_C( 0x0c0 ),				/* select 00 commands */
  U8X8_END_TRANSFER(),             	
  U8X8_END()             			/* end of sequence */
};

static const uint8_t u8x8_d_uc1628_flip1_seq[] = {
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8X8_C( 0x0c6 ),				
  U8X8_END_TRANSFER(),             	/* disable chip */
  U8X8_END()             			/* end of sequence */
};

static uint8_t u8x8_d_uc1628_generic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t x, c;
  uint8_t *ptr;
  switch(msg)
  {
    /* handled by the calling function
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
      u8x8_d_helper_display_setup_memory(u8x8, &u8x8_st75256_256x128_display_info);
      break;
    */
    /* handled by the calling function
    case U8X8_MSG_DISPLAY_INIT:
      u8x8_d_helper_display_init(u8x8);
      u8x8_cad_SendSequence(u8x8, u8x8_d_st75256_256x128_init_seq);    
      break;
    */
    case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
      if ( arg_int == 0 )
        u8x8_cad_SendSequence(u8x8, u8x8_d_uc1628_powersave0_seq);
      else
        u8x8_cad_SendSequence(u8x8, u8x8_d_uc1628_powersave1_seq);

      break;
#ifdef U8X8_WITH_SET_CONTRAST
    case U8X8_MSG_DISPLAY_SET_CONTRAST:

      u8x8_cad_StartTransfer(u8x8);
      
      u8x8_cad_SendCmd(u8x8, 0x081 );  /* volume control */
      u8x8_cad_SendArg(u8x8, 0 );	/* always 0 */
      u8x8_cad_SendArg(u8x8, arg_int);		/* 8 bit */
      
      u8x8_cad_EndTransfer(u8x8);
      break;
#endif
    case U8X8_MSG_DISPLAY_DRAW_TILE:
      
      u8x8_cad_StartTransfer(u8x8);
      x = ((u8x8_tile_t *)arg_ptr)->x_pos;    
      x *= 8;

      u8x8_cad_SendCmd(u8x8, 0x004 );	/* col */
      u8x8_cad_SendArg(u8x8, x);
    
      u8x8_cad_SendCmd(u8x8, 0x060 );	/* row */
      u8x8_cad_SendArg(u8x8, (((u8x8_tile_t *)arg_ptr)->y_pos));	
    
      u8x8_cad_SendCmd(u8x8, 0x001 );	        // write data
          
      do
      {
        c = ((u8x8_tile_t *)arg_ptr)->cnt;
        ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;
        /* SendData can not handle more than 255 bytes, treat c > 31 correctly  */
        if ( c > 31 )
        {
          u8x8_cad_SendData(u8x8, 248, ptr); 	/* 31*8=248 */
          ptr+=248;
          c -= 31;
        }
        
        u8x8_cad_SendData(u8x8, c*8, ptr); 	
        arg_int--;
      } while( arg_int > 0 );
      
      u8x8_cad_EndTransfer(u8x8);
      break;
    default:
      return 0;
  }
  return 1;
}

/*=============================================*/
/* 256x128, https://github.com/olikraus/u8g2/issues/2260 */

static const u8x8_display_info_t u8x8_uc1628_256x128_display_info =
{
  /* chip_enable_level = */ 0,
  /* chip_disable_level = */ 1,
  
  /* post_chip_enable_wait_ns = */ 10,
  /* pre_chip_disable_wait_ns = */ 10,
  /* reset_pulse_width_ms = */ 5, 	
  /* post_reset_wait_ms = */ 5, 		/**/
  /* sda_setup_time_ns = */ 25,		/* */
  /* sck_pulse_width_ns = */ 100,	/*  */
  /* sck_clock_hz = */ 4000000UL,	/* since Arduino 1.6.0, the SPI bus speed in Hz. Should be  1000000000/sck_pulse_width_ns */
  /* spi_mode = */ 0,		/* active high, rising edge */
  /* i2c_bus_clock_100kHz = */ 4,	/* 400KHz */
  /* data_setup_time_ns = */ 45,
  /* write_pulse_width_ns = */ 125,	
  /* tile_width = */ 32,
  /* tile_height = */ 16,
  /* default_x_offset = */ 0,	/* must be 0, because this is checked also for normal mode */
  /* flipmode_x_offset = */ 5,		/* used as y offset */
  /* pixel_width = */ 256,
  /* pixel_height = */ 128
};

static const uint8_t u8x8_d_uc1628_256x128_init_seq[] = {
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  
  U8X8_DLY(20),

  U8X8_C( 0x010 ),				/* */
  U8X8_C( 0x012 ),				/* */
  U8X8_C( 0x014 ),				/*  */
  
  U8X8_CAA( 0x016, 0x000, 0x00d ),				/*  */

  U8X8_CA( 0x040, 0x000 ),				/*  scroll line */

  U8X8_C( 0x020 ),				/* */
  U8X8_C( 0x02d ),				/* */
  U8X8_C( 0x0eb ),				/*  */

  U8X8_CAA( 0x081, 0x000, 0x056 ),				/*  */

  U8X8_CA( 0x0b8, 0x000 ),				/*   */

  U8X8_CA( 0x0f1, 0x07f ),				/*   */
  U8X8_CA( 0x0f2, 0x000 ),				/*   */
  U8X8_CA( 0x0f3, 0x07f ),				/*   */
  
  U8X8_C( 0x08f ),				/*   */

  U8X8_DLY(100),

  U8X8_END_TRANSFER(),             	/* disable chip */
  U8X8_END()             			/* end of sequence */
};

