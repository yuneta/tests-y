/****************************************************************************
 *          C_PEPON.H
 *          Pepon GClass.
 *
 *          Pepon, yuno server de pruebas
 *
 *          Copyright (c) 2018 by Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#ifndef _C_PEPON_H
#define _C_PEPON_H 1

#include <yuneta.h>

#ifdef __cplusplus
extern "C"{
#endif

/***************************************************************
 *              Constants
 ***************************************************************/
#define GCLASS_PEPON_NAME "Pepon"
#define GCLASS_PEPON gclass_pepon()

/***************************************************************
 *              Prototypes
 ***************************************************************/
PUBLIC GCLASS *gclass_pepon(void);

#ifdef __cplusplus
}
#endif

#endif
