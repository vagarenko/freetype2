/***************************************************************************/
/*                                                                         */
/*  ftserv.h                                                               */
/*                                                                         */
/*    The FreeType services (specification only).                          */
/*                                                                         */
/*  Copyright 2003 by                                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

  /*************************************************************************/
  /*                                                                       */
  /*  Each module can export one or more `services'.  Each service is      */
  /*  identified by a constant string and modeled by a pointer; the latter */
  /*  generally corresponds to a structure containing function pointers.   */
  /*                                                                       */
  /*  Note that a service's data cannot be a mere function pointer because */
  /*  in C it is possible that function pointers might be implemented      */
  /*  differently than data pointers (e.g. 48 bits instead of 32).         */
  /*                                                                       */
  /*************************************************************************/


#ifndef __FTSERV_H__
#define __FTSERV_H__


FT_BEGIN_HEADER


  /*
   * @macro:
   *   FT_FACE_FIND_SERVICE
   *
   * @description:
   *   This macro is used to lookup a service from a face's driver module.
   *
   * @input:
   *   id ::
   *     A string describing the service as defined in the service's
   *     header files (e.g. FT_SERVICE_ID_MULTI_MASTERS which expands to
   *     `multi-masters').
   *
   *   face ::
   *     The source face handle.
   *
   *   ptrtype ::
   *     The pointer type of `ptr'.  This is needed to make FreeType
   *     compile cleanly with C++.
   *
   * @output:
   *   ptr ::
   *     A variable that receives the service pointer.  Will be NULL
   *     if not found.
   */
#define FT_FACE_FIND_SERVICE( ptrtype, ptr, face, id )             \
  FT_BEGIN_STMNT                                                   \
    FT_Module  module = FT_MODULE( FT_FACE(face)->driver );        \
                                                                   \
                                                                   \
    (ptr) = NULL;                                                  \
    if ( module->clazz->get_interface )                            \
      (ptr) = (ptrtype)module->clazz->get_interface( module, id ); \
  FT_END_STMNT


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****         S E R V I C E   D E S C R I P T O R S                 *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/
 
  /*
   *  The following structure is used to _describe_ a given service
   *  to the library.  This is useful to build simple static service lists.
   */  
  typedef struct  FT_ServiceDescRec_
  {
    const char*  serv_id;     /* service name         */
    const void*  serv_data;   /* service pointer/data */
  
  } FT_ServiceDescRec;

  typedef const FT_ServiceDescRec*  FT_ServiceDesc;


  /*
   *  Parse a list of FT_ServiceDescRec descriptors and look for
   *  a specific service by ID.  Note that the last element in the
   *  array must be { NULL, NULL }, and that the function should
   *  return NULL if the service isn't available.
   *
   *  This function can be used by modules to implement their
   *  `get_service' method.
   */
  FT_BASE( FT_Pointer )
  ft_service_list_lookup( FT_ServiceDesc  service_descriptors,
                          const char*     service_id );


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****             S E R V I C E S   C A C H E                       *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/
 
  /*
   *  This structure is used to store a cache for several frequently used
   *  services.  It is the type of `face->internal->services'.  You
   *  should only use FT_FACE_LOOKUP_SERVICE to access it.
   *
   *  All fields should have the type FT_Pointer to relax compilation
   *  dependencies.  We assume the developer isn't completely stupid.
   */
  typedef struct  FT_ServiceCacheRec_
  {
    FT_Pointer  postscript_name;
    FT_Pointer  multi_masters;
    FT_Pointer  glyph_dict;
    
  } FT_ServiceCacheRec, *FT_ServiceCache;


  /*
   *  A magic number used within the services cache.
   */
#define FT_SERVICE_UNAVAILABLE  ((FT_Pointer)-2)  /* magic number */


  /*
   * @macro:
   *   FT_FACE_LOOKUP_SERVICE
   *
   * @description:
   *   This macro is used to lookup a service from a face's driver module
   *   using its cache.
   *
   * @input:
   *   face::
   *     The source face handle containing the cache.
   *
   *   field ::
   *     The field name in the cache.
   *
   *   id ::
   *     The service ID.
   *
   *   ptrtype ::
   *     The pointer type of `ptr'.  This is needed to make FreeType
   *     compile cleanly with C++.
   *
   * @output:
   *   ptr ::
   *     A variable receiving the service data.  NULL if not available.
   */
#define FT_FACE_LOOKUP_SERVICE( face, ptrtype, ptr, field, id ) \
  FT_BEGIN_STMNT                                                \
    (ptr) = (ptrtype)FT_FACE(face)->internal->services.field ;  \
    if ( (ptr) == FT_SERVICE_UNAVAILABLE )                      \
      (ptr) = NULL;                                             \
    else if ( (ptr) == NULL )                                   \
    {                                                           \
      FT_FACE_FIND_SERVICE( ptrtype, ptr, face, id );           \
                                                                \
      FT_FACE(face)->internal->services.field =                 \
        (FT_Pointer)( (ptr) != NULL ? (ptr)                     \
                                    : FT_SERVICE_UNAVAILABLE ); \
    }                                                           \
  FT_END_STMNT


  /*
   *  A macro used to define new service structure types.
   */

#define FT_DEFINE_SERVICE( name )            \
  typedef struct FT_Service_ ## name ## Rec_ \
    FT_Service_ ## name ## Rec ;             \
  typedef struct FT_Service_ ## name ## Rec_ \
    const * FT_Service_ ## name ;            \
  struct FT_Service_ ## name ## Rec_

  /* */

  /*
   *  The header files containing the services.
   */
 
#define FT_SERVICE_MULTIPLE_MASTERS_H  <freetype/internal/services/svmm.h>
#define FT_SERVICE_POSTSCRIPT_NAME_H   <freetype/internal/services/svpostnm.h> 
#define FT_SERVICE_GLYPH_DICT_H        <freetype/internal/services/svgldict.h>
#define FT_SERVICE_BDF_H               <freetype/internal/services/svbdf.h>
#define FT_SERVICE_XFREE86_NAME_H      <freetype/internal/services/svxf86nm.h>
#define FT_SERVICE_SFNT_H              <freetype/internal/services/svsfnt.h>


FT_END_HEADER

#endif /* __FTSERV_H__ */


/* END */