
/* 
 * $Id: v_optimize.h,v 1.2 2005/06/14 02:48:00 lxp Exp $
 */

/*
 * $Log: v_optimize.h,v $
 * Revision 1.2  2005/06/14 02:48:00  lxp
 * add new .c and .h by lixp, 2005-06-14
 *
 * Revision 1.1  2004/05/21 07:16:14  cy
 * [add] likely, unlikely
 *
 */

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
