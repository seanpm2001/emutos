/*
 *  kbq.c - keyboard queue management routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include	"portab.h"
#include	"bios.h"
#include	"kbd.h"
#include	"kbq.h"
#include	"kprint.h"

#define	KBQSIZE	128     /* number of entries to have in kbq */

/*
 * KBQUE - q structure.
 *
 * front and rear are indexes into the circular q starting at the
 * address indicated at base.  entries are unsigned words containing
 * the status and the scan code.  This module is not concerned about
 * which byte is which, only that the entries are words.
 */

#define	KBQUE	struct KbQueue
KBQUE
{
    int	kq_front ;		/*  index to front of q		*/
    int	kq_rear ;		/*  index to rear  of q		*/
    int	kq_size ;		/*  max size of queue		*/
    KBQENTRY *kq_base ;		/*  low addr of queue		*/
} ;


KBQENTRY kbqueue[ KBQSIZE ] ;		/*  the actual queue	*/
KBQUE	kbq;                            /*  logical queue	*/


/*==== Insert queue entry =================================================*/
/*
**  kbqadd - add an item into the keyboard queue
**	returns true if successful, false if queue full.
**
**	we use a copy of the pointer for two reasons:  so we may use reg
**	declarations, and so the delete routine is not
**	aware of the presence of a new item until it is securely in the
**	queue.
*/

BOOLEAN	kbq_add(UWORD item )     /*  item to put in queue	*/
{
    int		rear ;

    rear = (kbq.kq_rear+1) % kbq.kq_size ;	/* increase to next entry */
    if( kbq.kq_front == rear )		/*  check for full	*/
        return( FALSE ) ;
    *(kbq.kq_base+rear) = item ;		/*  insert in queue	*/
    kbq.kq_rear = rear ;			/*  update pointer	*/
    return( TRUE ) ;
}


/*==== Delete queue entry =================================================*/
/*
**  kbqdel - remove the front item of a queue and return it.
**	returns true if success, false if empty queue.  
**
**	we use a copy of the pointers for two reasons:  so we may use reg
**	declarations for the pointers, and so the add routine is not
**	aware of the delete (and thus try to use the slot with the item in
**	it) until it is securely removed from the queue.
*/

BOOLEAN	kbq_del(UWORD * itemp )  /*  ptr to where to put item	*/
{
    REG int	front ;

    /*  check for empty queue  */

    if( (front = kbq.kq_front) == kbq.kq_rear )
        return ( FALSE ) ;

    front = (front + 1) % kbq.kq_size ;
    *itemp = *(kbq.kq_base+front) ;
    kbq.kq_front = front ;
    return( TRUE ) ;
}



/*==== Initialize the keyboard queue ======================================*/
VOID	kbq_init()
{

    cputs("[    ] Keyboard queue initialized ...\r");
    kbq.kq_front = kbq.kq_rear = 0 ;    /* start and end */
    kbq.kq_size = KBQSIZE;		/* max size of queue */
    kbq.kq_base = kbqueue;		/* base address of queue */

    cstatus(SUCCESS);
}
