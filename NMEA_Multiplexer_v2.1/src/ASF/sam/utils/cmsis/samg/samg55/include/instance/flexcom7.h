/**
 * \file
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _SAMG55_FLEXCOM7_INSTANCE_
#define _SAMG55_FLEXCOM7_INSTANCE_

/* ========== Register definition for FLEXCOM7 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_FLEXCOM7_MR                   (0x40034000U) /**< \brief (FLEXCOM7) FLEXCOM Mode register */
  #define REG_FLEXCOM7_RHR                  (0x40034010U) /**< \brief (FLEXCOM7) FLEXCOM Receive Holding Register */
  #define REG_FLEXCOM7_THR                  (0x40034020U) /**< \brief (FLEXCOM7) FLEXCOM Transmit Holding Register */
#else
  #define REG_FLEXCOM7_MR  (*(__IO uint32_t*)0x40034000U) /**< \brief (FLEXCOM7) FLEXCOM Mode register */
  #define REG_FLEXCOM7_RHR (*(__I  uint32_t*)0x40034010U) /**< \brief (FLEXCOM7) FLEXCOM Receive Holding Register */
  #define REG_FLEXCOM7_THR (*(__IO uint32_t*)0x40034020U) /**< \brief (FLEXCOM7) FLEXCOM Transmit Holding Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAMG55_FLEXCOM7_INSTANCE_ */