/****************************************************************************
 *          C_ROOT.H
 *          Root GClass.
 *
 *          GClass for testing
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
#define GCLASS_ROOT_NAME "Root"
#define GCLASS_ROOT gclass_root()

/***************************************************************
 *              Prototypes
 ***************************************************************/
PUBLIC GCLASS *gclass_root(void);

#ifdef __cplusplus
}
#endif
