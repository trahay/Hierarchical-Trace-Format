/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2021,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_COMPILER_H
#define OTF2_COMPILER_H


/**
 *  @file
 *
 *  @brief      Compiler feature test macros.
 */


#if defined( __has_attribute )
  #define __otf2_has_attribute( x ) __has_attribute( x )
#else
  #define __otf2_has_attribute( x ) 0
#endif


#if __otf2_has_attribute( deprecated ) && !defined( OTF2_IGNORE_ATTRIBUTE_DEPRECATED )
  // __has_attribute was introduced in GCC 5 but attributes on enum entries only in GCC 6
  // PGI/NVIDIA still claim GCC compatibility, but this is a lie
  #if !( defined( __PGI ) || defined( __GNUC__ ) && __GNUC__ < 6 )
    #define __otf2_deprecated__ __attribute__( ( deprecated ) )
  #endif
#endif


#ifndef __otf2_deprecated__
  #define __otf2_deprecated__
#endif


#undef __otf2_has_attribute


#endif /* !OTF2_COMPILER_H */
