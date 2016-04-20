/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/*  (C)Copyright IBM Corp.  2007, 2011  */
/**
 * \file src/coll/exscan/mpido_iexscan.c
 * \brief ???
 */

/*#define TRACE_ON */
#include <mpidimpl.h>

int MPIDO_Iexscan(const void *sendbuf, void *recvbuf,
                  int count, MPI_Datatype datatype,
                  MPI_Op op, MPIR_Comm * comm_ptr, MPIR_Request **request)
{
   /*if (unlikely((data_size == 0) || (user_selected_type == MPID_COLL_USE_MPICH)))*/
   {
      /*
       * If the mpich mpir non-blocking collectives are enabled, return without
       * first constructing the MPIR_Request. This signals to the
       * MPIR_Iexscan_impl() function to invoke the mpich nbc implementation
       * of MPI_Iexscan().
       */
      if (MPIDI_Process.mpir_nbc != 0)
       return 0;

      /*
       * MPIR_* nbc implementation is not enabled. Fake a non-blocking
       * MPIR_Iexscan() with a blocking MPIR_Exscan().
       */
      if(unlikely(MPIDI_Process.verbose >= MPIDI_VERBOSE_DETAILS_ALL && comm_ptr->rank == 0))
         fprintf(stderr,"Using MPICH blocking exscan algorithm\n");

      int mpierrno = 0;
      int rc = MPIR_Exscan_impl(sendbuf, recvbuf, count, datatype, op, comm_ptr, &mpierrno);

      /*
       * The blocking exscan has completed - create and complete a
       * MPIR_Request object so the MPIR_Iexscan_impl() function does not
       * perform an additional iexscan.
       */
      MPIR_Request * mpid_request = MPID_Request_create_inline();
      mpid_request->kind = MPIR_REQUEST_KIND__COLL;
      *request = mpid_request;
      MPIDI_Request_complete_norelease_inline(mpid_request);

      return rc;
   }

   return 0;
}
