/*
 * Geoclue
 * geoclue-mock.c - Mock location provider
 *
 * Author: Santtu Lakkala <inz@inz.fi>
 * Copyright 2016 Santtu Lakkala
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <config.h>

#include <stdlib.h>

#include <geoclue/gc-provider.h>
#include <geoclue/gc-iface-position.h>

#include <gio/gio.h>
#include <gobject/gobject.h>

enum mock_properties {
	PROP_LATITUDE = 1,
	PROP_LONGITUDE,
	PROP_FUZZ,
	PROP_ACTIVE,
	PROP_COUNT
};

typedef struct {
	GcProvider parent;

	GMainLoop *loop;

	guint timer;
	gboolean active;
	double latitude;
	double longitude;
	double fuzz;
} GeoclueMock;

typedef struct {
	GcProviderClass parent_class;
} GeoclueMockClass;

static GType geoclue_mock_get_type(void);
#define GEOCLUE_TYPE_MOCK (geoclue_mock_get_type ())
#define GEOCLUE_MOCK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEOCLUE_TYPE_MOCK, GeoclueMock))

static void geoclue_mock_position_init(GcIfacePositionClass *iface);
static void geoclue_mock_set_property(GObject *object,
		guint property_id,
		const GValue *value,
		GParamSpec *pspec);
static void geoclue_mock_get_property(GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec);

G_DEFINE_TYPE_WITH_CODE(GeoclueMock, geoclue_mock, GC_TYPE_PROVIDER,
			G_IMPLEMENT_INTERFACE(GC_TYPE_IFACE_POSITION,
					      geoclue_mock_position_init))

static gboolean _geoclue_mock_get_status(GcIfaceGeoclue *gc,
					 GeoclueStatus *status,
					 GError **error)
{
	(void)gc;
	(void)error;

	*status = GEOCLUE_STATUS_AVAILABLE;

	return TRUE;
}

static gboolean _geoclue_mock_set_options(GcIfaceGeoclue *gc,
					  GHashTable *options,
					  GError **error)
{
	(void)gc;
	(void)error;
	(void)options;

	return TRUE;
}

static void _geoclue_mock_shutdown(GcProvider *provider)
{
	GeoclueMock *mock = GEOCLUE_MOCK(provider);

	g_main_loop_quit(mock->loop);
}

static void geoclue_mock_class_init(GeoclueMockClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GcProviderClass *p_class = (GcProviderClass *) klass;

	p_class->get_status = _geoclue_mock_get_status;
	p_class->set_options = _geoclue_mock_set_options;
	p_class->shutdown = _geoclue_mock_shutdown;

	object_class->get_property = geoclue_mock_get_property;
	object_class->set_property = geoclue_mock_set_property;

	g_object_class_install_property(object_class,
			PROP_LATITUDE,
			g_param_spec_double(
					"latitude",
					"latitude",
					"latitude",
					-180.0, 180.0, 0.0,
					G_PARAM_WRITABLE | G_PARAM_READABLE));
	g_object_class_install_property(object_class,
			PROP_LONGITUDE,
			g_param_spec_double(
					"longitude",
					"longitude",
					"longitude",
					-180.0, 180.0, 0.0,
					G_PARAM_WRITABLE | G_PARAM_READABLE));
	g_object_class_install_property(object_class,
			PROP_FUZZ,
			g_param_spec_double(
					"fuzz",
					"fuzz",
					"fuzz",
					0.0, 180.0, 0.0,
					G_PARAM_WRITABLE | G_PARAM_READABLE));
	g_object_class_install_property(object_class,
			PROP_ACTIVE,
			g_param_spec_boolean(
					"active",
					"active",
					"active",
					FALSE,
					G_PARAM_WRITABLE | G_PARAM_READABLE));
}

static gboolean _geoclue_mock_emit_position_changed(gpointer data)
{
	GeoclueMock *mock = data;
	GeoclueAccuracy *accuracy;

	accuracy = geoclue_accuracy_new(mock->active ?
				     GEOCLUE_ACCURACY_LEVEL_DETAILED :
				     GEOCLUE_ACCURACY_LEVEL_NONE,
				     15.0, 60.0);

	gc_iface_position_emit_position_changed
		(GC_IFACE_POSITION(mock),
		 mock->active ? (GEOCLUE_POSITION_FIELDS_LATITUDE |
				    GEOCLUE_POSITION_FIELDS_LONGITUDE) :
		 GEOCLUE_POSITION_FIELDS_NONE,
		 time(NULL),
		 mock->latitude + (rand() * 1.0 / RAND_MAX -
				      0.5) * mock->fuzz,
		 mock->longitude + (rand() * 1.0 / RAND_MAX -
				       0.5) * mock->fuzz,
		 0.0, accuracy);

	geoclue_accuracy_free(accuracy);

	mock->timer =
		g_timeout_add_seconds(5,
				      _geoclue_mock_emit_position_changed,
				      mock);

	return G_SOURCE_REMOVE;
}

static void geoclue_mock_set_property(GObject *object,
		guint property_id,
		const GValue *value,
		GParamSpec *pspec)
{
	GeoclueMock *mock = GEOCLUE_MOCK(object);

	switch (property_id) {
	case PROP_LATITUDE:
		mock->latitude = g_value_get_double(value);
		break;
	case PROP_LONGITUDE:
		mock->longitude = g_value_get_double(value);
		break;
	case PROP_FUZZ:
		mock->fuzz = g_value_get_double(value);
		break;
	case PROP_ACTIVE:
		mock->active = g_value_get_boolean(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object,
				property_id,
				pspec);
		return;
	}

	if (mock->timer)
		g_source_remove(mock->timer);
	mock->timer = g_timeout_add(100,
				    _geoclue_mock_emit_position_changed,
				    mock);
}

static void geoclue_mock_get_property(GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec)
{
	GeoclueMock *mock = GEOCLUE_MOCK(object);

	switch (property_id) {
	case PROP_LATITUDE:
		g_value_set_double(value, mock->latitude);
		break;
	case PROP_LONGITUDE:
		g_value_set_double(value, mock->longitude);
		break;
	case PROP_FUZZ:
		g_value_set_double(value, mock->fuzz);
		break;
	case PROP_ACTIVE:
		g_value_set_boolean(value, mock->active);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object,
				property_id,
				pspec);
		break;
	}
}

static void geoclue_mock_init(GeoclueMock *mock)
{
	gc_provider_set_details(GC_PROVIDER(mock),
				"org.freedesktop.Geoclue.Providers.Mock",
				"/org/freedesktop/Geoclue/Providers/Mock",
				"Mock", "Mock provider");
	mock->timer =
		g_timeout_add_seconds(5,
				      _geoclue_mock_emit_position_changed,
				      mock);
}

static gboolean _geoclue_mock_get_position(GcIfacePosition *gc,
					   GeocluePositionFields *fields,
					   int *timestamp,
					   double *latitude,
					   double *longitude,
					   double *altitude,
					   GeoclueAccuracy **accuracy,
					   GError **error)
{
	GeoclueMock *mock = GEOCLUE_MOCK(gc);

	(void)altitude;
	(void)error;

	*timestamp = time(NULL);

	*fields = mock->active ? (GEOCLUE_POSITION_FIELDS_LATITUDE |
				  GEOCLUE_POSITION_FIELDS_LONGITUDE) :
		GEOCLUE_POSITION_FIELDS_NONE, *accuracy =
		geoclue_accuracy_new((mock->active ?
				      GEOCLUE_ACCURACY_LEVEL_DETAILED :
				      GEOCLUE_ACCURACY_LEVEL_NONE),
				     15.0, 60.0);

	*latitude = mock->latitude;
	*longitude = mock->longitude;

	return TRUE;
}

static void geoclue_mock_position_init(GcIfacePositionClass *iface)
{
	iface->get_position = _geoclue_mock_get_position;
}

int main(int argc, char **argv)
{
	GeoclueMock *mock;
	GSettings *settings;

	(void)argc;
	(void)argv;

#ifndef GLIB_VERSION_2_36
	g_type_init();
#endif

	srand(time(NULL));
	mock = g_object_new(GEOCLUE_TYPE_MOCK, NULL);

	mock->loop = g_main_loop_new(NULL, TRUE);
	settings = g_settings_new("fi.inz.GeoclueMock");
	g_settings_bind(settings, "active",
			mock, "active",
			G_SETTINGS_BIND_DEFAULT |
			G_SETTINGS_BIND_NO_SENSITIVITY);
	g_settings_bind(settings, "latitude",
			mock, "latitude",
			G_SETTINGS_BIND_DEFAULT |
			G_SETTINGS_BIND_NO_SENSITIVITY);
	g_settings_bind(settings, "fuzz",
			mock, "fuzz",
			G_SETTINGS_BIND_DEFAULT |
			G_SETTINGS_BIND_NO_SENSITIVITY);
	g_settings_bind(settings, "longitude",
			mock, "longitude",
			G_SETTINGS_BIND_DEFAULT |
			G_SETTINGS_BIND_NO_SENSITIVITY);

	g_main_loop_run(mock->loop);

	return 0;
}
