/* GTK - The GIMP Toolkit
 * Copyright (C) 2010 Carlos Garnacho <carlosg@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "gtkstyleproviderprivate.h"

#include "gtkprivate.h"

/**
 * GtkStyleProvider:
 *
 * `GtkStyleProvider` is an interface for style information used by
 * `GtkStyleContext`.
 *
 * See [method@Gtk.StyleContext.add_provider] and
 * [func@Gtk.StyleContext.add_provider_for_display] for
 * adding `GtkStyleProviders`.
 *
 * GTK uses the `GtkStyleProvider` implementation for CSS in
 * [class@Gtk.CssProvider].
 */

enum {
  CHANGED,
  LAST_SIGNAL
};

G_DEFINE_INTERFACE (GtkStyleProvider, gtk_style_provider, G_TYPE_OBJECT)

static guint signals[LAST_SIGNAL];

static void
gtk_style_provider_default_init (GtkStyleProviderInterface *iface)
{
  signals[CHANGED] = g_signal_new (I_("gtk-private-changed"),
                                   G_TYPE_FROM_INTERFACE (iface),
                                   G_SIGNAL_RUN_LAST,
                                   G_STRUCT_OFFSET (GtkStyleProviderInterface, changed),
                                   NULL, NULL,
                                   NULL,
                                   G_TYPE_NONE, 0);

}

GtkCssValue *
gtk_style_provider_get_color (GtkStyleProvider *provider,
                              const char       *name)
{
  GtkStyleProviderInterface *iface;

  /* for compat with gtk_symbolic_color_resolve() */
  if (provider == NULL)
    return NULL;

  gtk_internal_return_val_if_fail (GTK_IS_STYLE_PROVIDER (provider), NULL);

  iface = GTK_STYLE_PROVIDER_GET_INTERFACE (provider);

  if (!iface->get_color)
    return NULL;

  return iface->get_color (provider, name);
}

GtkCssKeyframes *
gtk_style_provider_get_keyframes (GtkStyleProvider *provider,
                                  const char       *name)
{
  GtkStyleProviderInterface *iface;

  gtk_internal_return_val_if_fail (GTK_IS_STYLE_PROVIDER (provider), NULL);
  gtk_internal_return_val_if_fail (name != NULL, NULL);

  iface = GTK_STYLE_PROVIDER_GET_INTERFACE (provider);

  if (!iface->get_keyframes)
    return NULL;

  return iface->get_keyframes (provider, name);
}

void
gtk_style_provider_lookup (GtkStyleProvider             *provider,
                           const GtkCountingBloomFilter *filter,
                           GtkCssNode                   *node,
                           GtkCssLookup                 *lookup,
                           GtkCssChange                 *out_change)
{
  GtkStyleProviderInterface *iface;

  gtk_internal_return_if_fail (GTK_IS_STYLE_PROVIDER (provider));
  gtk_internal_return_if_fail (GTK_IS_CSS_NODE (node));
  gtk_internal_return_if_fail (lookup != NULL);

  if (out_change)
    *out_change = 0;

  iface = GTK_STYLE_PROVIDER_GET_INTERFACE (provider);

  if (!iface->lookup)
    return;

  iface->lookup (provider, filter, node, lookup, out_change);
}

void
gtk_style_provider_changed (GtkStyleProvider *provider)
{
  gtk_internal_return_if_fail (GTK_IS_STYLE_PROVIDER (provider));

  g_signal_emit (provider, signals[CHANGED], 0);
}

GtkSettings *
gtk_style_provider_get_settings (GtkStyleProvider *provider)
{
  GtkStyleProviderInterface *iface;

  gtk_internal_return_val_if_fail (GTK_IS_STYLE_PROVIDER (provider), NULL);

  iface = GTK_STYLE_PROVIDER_GET_INTERFACE (provider);

  if (!iface->get_settings)
    return NULL;

  return iface->get_settings (provider);
}

int
gtk_style_provider_get_scale (GtkStyleProvider *provider)
{
  GtkStyleProviderInterface *iface;

  gtk_internal_return_val_if_fail (GTK_IS_STYLE_PROVIDER (provider), 1);

  iface = GTK_STYLE_PROVIDER_GET_INTERFACE (provider);

  if (!iface->get_scale)
    return 1;

  return iface->get_scale (provider);
}

void
gtk_style_provider_emit_error (GtkStyleProvider *provider,
                               GtkCssSection    *section,
                               GError           *error)
{
  GtkStyleProviderInterface *iface;

  iface = GTK_STYLE_PROVIDER_GET_INTERFACE (provider);

  if (iface->emit_error)
    iface->emit_error (provider, section, error);
}
