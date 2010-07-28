/*
 * peas-introspection.c
 * This file is part of libpeas
 *
 * Copyright (C) 2010 Steve Frécinaux
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "peas-introspection.h"

static gboolean
read_next_argument (GArgument  *cur_arg,
                    va_list    *args,
                    GITypeInfo *arg_type_info)
{
  /* Notes:
   * According to GCC 4.4,
   *  - int8, uint8, int16, uint16, short and ushort are promoted to int when
   *    passed through '...'
   *  - float is promoted to double when passed through '...'
   *
   * According to 'man va_arg' (and the C99 spec),
   *  "If a va_list is passed to a function that uses va_arg(ap,type) then the
   *   value of the va_list is undefined after the return of that function."
   * To work around this, we pass a pointer to the va_list to this function.
   */
  switch (g_type_info_get_tag (arg_type_info))
    {
    case GI_TYPE_TAG_VOID:
    case GI_TYPE_TAG_BOOLEAN:
      cur_arg->v_boolean = va_arg (*args, gboolean);
      break;
    case GI_TYPE_TAG_INT8:
      cur_arg->v_int8 = va_arg (*args, gint);
      break;
    case GI_TYPE_TAG_UINT8:
      cur_arg->v_uint8 = va_arg (*args, gint);
      break;
    case GI_TYPE_TAG_INT16:
      cur_arg->v_int16 = va_arg (*args, gint);
      break;
    case GI_TYPE_TAG_UINT16:
      cur_arg->v_uint16 = va_arg (*args, gint);
      break;
    case GI_TYPE_TAG_INT32:
      cur_arg->v_int32 = va_arg (*args, gint32);
      break;
    case GI_TYPE_TAG_UINT32:
      cur_arg->v_uint32 = va_arg (*args, guint32);
      break;
    case GI_TYPE_TAG_INT64:
      cur_arg->v_int64 = va_arg (*args, gint64);
      break;
    case GI_TYPE_TAG_UINT64:
      cur_arg->v_uint64 = va_arg (*args, guint64);
      break;
    case GI_TYPE_TAG_FLOAT:
      cur_arg->v_float = va_arg (*args, gdouble);
      break;
    case GI_TYPE_TAG_DOUBLE:
      cur_arg->v_double = va_arg (*args, gdouble);
      break;
    case GI_TYPE_TAG_GTYPE:
      /* apparently, GType is meant to be a gsize, from gobject/gtype.h in glib */
      cur_arg->v_size = va_arg (*args, GType);
      break;
    case GI_TYPE_TAG_UTF8:
    case GI_TYPE_TAG_FILENAME:
      cur_arg->v_string = va_arg (*args, gchar *);
      break;
    case GI_TYPE_TAG_ARRAY:
    case GI_TYPE_TAG_INTERFACE:
    case GI_TYPE_TAG_GLIST:
    case GI_TYPE_TAG_GSLIST:
    case GI_TYPE_TAG_GHASH:
    case GI_TYPE_TAG_ERROR:
      cur_arg->v_pointer = va_arg (*args, gpointer);
      break;
    default:
      g_return_val_if_reached (FALSE);
    }

  return TRUE;
}

static gboolean
set_return_value (gpointer    in_retval,
                  GArgument  *out_retval,
                  GITypeInfo *retval_type_info)
{
  /* We should discourage people to actually use NULL for the return value. */
  g_return_val_if_fail (in_retval != NULL, TRUE);

  switch (g_type_info_get_tag (retval_type_info))
    {
    case GI_TYPE_TAG_VOID:
    case GI_TYPE_TAG_BOOLEAN:
      *((gboolean *) in_retval) = out_retval->v_boolean;
      break;
    case GI_TYPE_TAG_INT8:
      *((gint8 *) in_retval) = out_retval->v_int8;
      break;
    case GI_TYPE_TAG_UINT8:
      *((guint8 *) in_retval) = out_retval->v_uint8;
      break;
    case GI_TYPE_TAG_INT16:
      *((gint16 *) in_retval) = out_retval->v_int16;
      break;
    case GI_TYPE_TAG_UINT16:
      *((guint16 *) in_retval) = out_retval->v_uint16;
      break;
    case GI_TYPE_TAG_INT32:
      *((gint32 *) in_retval) = out_retval->v_int32;
      break;
    case GI_TYPE_TAG_UINT32:
      *((guint32 *) in_retval) = out_retval->v_uint32;
      break;
    case GI_TYPE_TAG_INT64:
      *((gint64 *) in_retval) = out_retval->v_int64;
      break;
    case GI_TYPE_TAG_UINT64:
      *((guint64 *) in_retval) = out_retval->v_uint64;
      break;
    case GI_TYPE_TAG_FLOAT:
      *((gfloat *) in_retval) = out_retval->v_float;
      break;
    case GI_TYPE_TAG_DOUBLE:
      *((gdouble *) in_retval) = out_retval->v_double;
      break;
    case GI_TYPE_TAG_GTYPE:
      /* apparently, GType is meant to be a gsize, from gobject/gtype.h in glib */
      *((gsize *) in_retval) = out_retval->v_size;
      break;
    case GI_TYPE_TAG_UTF8:
    case GI_TYPE_TAG_FILENAME:
      *((gchar **) in_retval) = out_retval->v_string;
      break;
    case GI_TYPE_TAG_ARRAY:
    case GI_TYPE_TAG_INTERFACE:
    case GI_TYPE_TAG_GLIST:
    case GI_TYPE_TAG_GSLIST:
    case GI_TYPE_TAG_GHASH:
    case GI_TYPE_TAG_ERROR:
      *((gsize **) in_retval) = out_retval->v_pointer;
      break;
    default:
      g_return_val_if_reached (FALSE);
    }

  return TRUE;
}

GICallableInfo *
peas_method_get_info (GType        iface_type,
                      const gchar *method_name)
{
  GIRepository *repo;
  GIBaseInfo *iface_info;
  GIFunctionInfo *func_info;

  repo = g_irepository_get_default ();
  iface_info = g_irepository_find_by_gtype (repo, iface_type);
  if (iface_info == NULL)
    {
      g_warning ("Type not found in introspection: '%s'",
                 g_type_name (iface_type));
      return NULL;
    }

  switch (g_base_info_get_type (iface_info))
    {
    case GI_INFO_TYPE_OBJECT:
      func_info = g_object_info_find_method ((GIObjectInfo *) iface_info,
                                             method_name);
      break;
    case GI_INFO_TYPE_INTERFACE:
      func_info = g_interface_info_find_method ((GIInterfaceInfo *) iface_info,
                                                method_name);
      break;
    default:
      func_info = NULL;
    }

  if (func_info == NULL)
    {
      g_warning ("Method '%s.%s' not found",
                 g_type_name (iface_type),
                 method_name);
    }

  g_base_info_unref (iface_info);
  return (GICallableInfo *) func_info;
}

gboolean
peas_method_apply_valist (GObject     *instance,
                          GType        iface_type,
                          const gchar *method_name,
                          va_list      args)
{
  GICallableInfo *func_info;
  GITypeInfo *retval_info;
  GArgument out_retval;
  guint n_args, n_in_args, n_out_args;
  GArgument *in_args, *out_args;
  guint i;
  gboolean ret = TRUE;
  GError *error = NULL;

  func_info = peas_method_get_info (iface_type, method_name);
  if (func_info == NULL)
    return FALSE;

  retval_info = g_callable_info_get_return_type (func_info);

  n_args = g_callable_info_get_n_args (func_info);
  n_in_args = 0;
  n_out_args = 0;

  in_args = g_new0 (GArgument, n_args + 1);
  out_args = g_new0 (GArgument, n_args);

  /* Set the object as the first argument for the method. */
  in_args[n_in_args++].v_pointer = instance;

  for (i = 0; ret && i < n_args; i++)
    {
      GIArgInfo *arg_info;
      GITypeInfo *arg_type_info;

      arg_info = g_callable_info_get_arg (func_info, i);
      arg_type_info = g_arg_info_get_type (arg_info);

      switch (g_arg_info_get_direction (arg_info))
        {
        case GI_DIRECTION_IN:
          ret = read_next_argument (&in_args[n_in_args++], &args, arg_type_info);
          break;
        /* In the other cases, we expect we will always have a pointer. */
        case GI_DIRECTION_INOUT:
          in_args[n_in_args++].v_pointer = out_args[n_out_args++].v_pointer = va_arg (args, gpointer);
          break;
        case GI_DIRECTION_OUT:
          out_args[n_out_args++].v_pointer = va_arg (args, gpointer);
          break;
        }

      g_base_info_unref ((GIBaseInfo *) arg_type_info);
      g_base_info_unref ((GIBaseInfo *) arg_info);
    }

  if (!ret)
    {
      g_warning ("Invalid argument type for calling '%s.%s'",
                 g_type_name (iface_type), method_name);
      goto out;
    }

  g_debug ("Calling '%s.%s' on '%p'",
           g_type_name (iface_type), method_name, instance);

  ret = g_function_info_invoke (func_info, in_args, n_in_args, out_args,
                                n_out_args, &out_retval, &error);
  if (!ret)
    {
      g_warning ("Error while calling '%s.%s': %s",
                 g_type_name (iface_type), method_name, error->message);
      g_error_free (error);
      goto out;
    }

  if (g_type_info_get_tag (retval_info) != GI_TYPE_TAG_VOID)
    {
      ret = set_return_value (va_arg (args, gpointer), &out_retval, retval_info);

      if (!ret)
        {
          g_warning ("Invalid return type for '%s.%s'",
                     g_type_name (iface_type), method_name);
        }
    }

out:
  g_free (in_args);
  g_free (out_args);
  g_base_info_unref ((GIBaseInfo *) retval_info);
  g_base_info_unref ((GIBaseInfo *) func_info);

  return ret;
}
