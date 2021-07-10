/****************************************************************************
 *          C_TESTON.H
 *          Teston GClass.
 *
 *          Teston, yuno client de pruebas
 *
 *          Copyright (c) 2018 by Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#pragma once

#include <yuneta.h>

#ifdef __cplusplus
extern "C"{
#endif

/***************************************************************
 *              Constants
 ***************************************************************/
#define GCLASS_TESTON_NAME "Teston"
#define GCLASS_TESTON gclass_teston()

/***************************************************************
 *              Prototypes
 ***************************************************************/
PUBLIC GCLASS *gclass_teston(void);

#ifdef __cplusplus
}
#endif
