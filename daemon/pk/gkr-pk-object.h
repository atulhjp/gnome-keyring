/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gkr-pk-object.h - A base class for PK objects

   Copyright (C) 2007 Stefan Walter

   The Gnome Keyring Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Keyring Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Stef Walter <stef@memberwebs.com>
*/

#ifndef __GKR_PK_OBJECT_H__
#define __GKR_PK_OBJECT_H__

#include <glib-object.h>
#include <pkcs11/pkcs11.h>

#include "common/gkr-id.h"

G_BEGIN_DECLS

#define GKR_PK_INDEX_LABEL              "label"

#define GKR_PK_OBJECT_HANDLE_MASK       0x0FFFFFFF
#define GKR_PK_OBJECT_IS_PERMANENT	0x10000000
#define GKR_PK_OBJECT_IS_TEMPORARY	0x00000000

#define GKR_TYPE_PK_OBJECT             (gkr_pk_object_get_type())
#define GKR_PK_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GKR_TYPE_PK_OBJECT, GkrPkObject))
#define GKR_PK_OBJECT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GKR_TYPE_PK_OBJECT, GkrPkObjectClass))
#define GKR_IS_PK_OBJECT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GKR_TYPE_PK_OBJECT))
#define GKR_IS_PK_OBJECT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GKR_TYPE_PK_OBJECT))
#define GKR_PK_OBJECT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GKR_TYPE_PK_OBJECT, GkrPkObjectClass))

typedef struct _GkrPkObject      GkrPkObject;
typedef struct _GkrPkObjectClass GkrPkObjectClass;

typedef struct _GkrPkStorage GkrPkStorage;
typedef struct _GkrPkManager GkrPkManager;
typedef struct _GkrPkSession GkrPkSession;

struct _GkrPkObject {
	GObject parent;
	
	GQuark location;
	gkrid digest;
	CK_OBJECT_HANDLE handle;
	
	GkrPkManager *manager;
	GkrPkStorage *storage;
};

struct _GkrPkObjectClass {
	GObjectClass parent_class;

	/* 
	 * The attribute getter and setter for PKCS#11 attributes.
	 * The base class getters and setters provide access to common
	 * attributes, and derived classes override. 
	 */
	CK_RV (*get_attribute) (GkrPkObject *obj, CK_ATTRIBUTE_PTR attr);
	CK_RV (*set_attribute) (GkrPkObject *obj, CK_ATTRIBUTE_PTR attr); 
	
	/*
	 * Overridden by derived classes to prepare an object for 
	 * use by gnome-keyring. This might include extracting public
	 * key and storing it in indexes or other stuff like that.
	 */
	gboolean (*import) (GkrPkObject *obj);
	
	/* 
	 * Overridden by derived classes to provide the serialized 
	 * representation of the object, minus modifiable attributes, and 
	 * things that go in the index. 
	 * 
	 * Objects may choose to use the 'password' to encrypt and/or MAC 
	 * the data, as long as the representation is later decryptable. 
	 */
	guchar* (*serialize) (GkrPkObject *obj, const gchar* password, gsize *n_data);
	
	/* 
	 * Asks the object to lock itself, ie: remove any sensitive data from 
	 * memory.
	 */
	void (*lock) (GkrPkObject *obj);
};

GType               gkr_pk_object_get_type         (void) G_GNUC_CONST;

GType               gkr_pk_object_get_object_type  (GQuark pkix_type);

CK_RV               gkr_pk_object_create           (GkrPkSession *session,
                                                    GArray *attrs, GkrPkObject **object);
                                                    
void                gkr_pk_object_refresh          (GkrPkObject *object);

void                gkr_pk_object_flush            (GkrPkObject *object);

void                gkr_pk_object_lock             (GkrPkObject *object);

gboolean            gkr_pk_object_import           (GkrPkObject *object);

gboolean            gkr_pk_object_match            (GkrPkObject *object,
                                                    GArray *attrs);
                                                    
gboolean            gkr_pk_object_match_one        (GkrPkObject *object,
                                                    CK_ATTRIBUTE_PTR attr);

CK_OBJECT_HANDLE    gkr_pk_object_get_handle       (GkrPkObject *object);

GkrPkManager*       gkr_pk_object_get_manager      (GkrPkObject *object);

CK_RV               gkr_pk_object_get_attribute    (GkrPkObject *object,
                                                    CK_ATTRIBUTE_PTR attr);
                                                    
CK_RV               gkr_pk_object_get_ulong        (GkrPkObject *object,
                                                    CK_ATTRIBUTE_TYPE type,
                                                    CK_ULONG *value);
                                                    
CK_RV               gkr_pk_object_get_bool         (GkrPkObject *object,
                                                    CK_ATTRIBUTE_TYPE type,
                                                    CK_BBOOL *value);
                                                    
CK_RV               gkr_pk_object_get_attributes   (GkrPkObject *object,
                                                    GArray *attrs);

CK_RV               gkr_pk_object_set_attribute    (GkrPkObject *object,
                                                    CK_ATTRIBUTE_PTR attr);
                                                    
CK_RV               gkr_pk_object_set_ulong        (GkrPkObject *object, 
                                                    CK_ATTRIBUTE_TYPE type,
                                                    CK_ULONG value);

CK_RV               gkr_pk_object_set_bool         (GkrPkObject *object, 
                                                    CK_ATTRIBUTE_TYPE type,
                                                    CK_BBOOL value);
                                                    
CK_RV               gkr_pk_object_set_attributes   (GkrPkObject *object, 
                                                    GArray *attrs);

gboolean            gkr_pk_object_has_label        (GkrPkObject *object);

const gchar*        gkr_pk_object_get_label        (GkrPkObject *object);

void                gkr_pk_object_set_label        (GkrPkObject *object, 
                                                    const gchar *label);

/* -------------------------------------------------------------------
 * Helpers to access the index for an object 
 */

gboolean            gkr_pk_object_index_has_value    (GkrPkObject *object,
                                                      const gchar *field);

gboolean            gkr_pk_object_index_get_boolean  (GkrPkObject *object,
                                                      const gchar *field);

GQuark*             gkr_pk_object_index_get_quarks   (GkrPkObject *object,
                                                      const gchar *field);

gchar*              gkr_pk_object_index_get_string   (GkrPkObject *object,
                                                      const gchar *field);

guchar*             gkr_pk_object_index_get_binary   (GkrPkObject *object,
                                                      const gchar *field,
                                                      gsize *n_data);

void                gkr_pk_object_index_set_boolean  (GkrPkObject *object,
                                                      const gchar *field,
                                                      gboolean value);

void                gkr_pk_object_index_set_string   (GkrPkObject *object,
                                                      const gchar *field,
                                                      const gchar *string);

void                gkr_pk_object_index_set_binary   (GkrPkObject *object,
                                                      const gchar *field,
                                                      const guchar *data,
                                                      gsize n_data);

void                gkr_pk_object_index_clear        (GkrPkObject *object, 
                                                      const gchar *field);

G_END_DECLS

#endif /* __GKR_PK_OBJECT_H__ */
