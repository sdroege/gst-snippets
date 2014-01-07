/*
 * Simple GStreamer application template
 *
 * Copyright (c) 2014 Sebastian Dröge <sebastian@centricular.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <gst/gst.h>

static GMainLoop *loop;
static GstElement *pipeline;
static GstElement *src, *conv, *sink;

static gboolean
message_cb (GstBus * bus, GstMessage * message, gpointer user_data)
{
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err = NULL;
      gchar *name, *debug = NULL;

      name = gst_object_get_path_string (message->src);
      gst_message_parse_error (message, &err, &debug);

      g_printerr ("ERROR: from element %s: %s\n", name, err->message);
      if (debug != NULL)
        g_printerr ("Additional debug info:\n%s\n", debug);

      g_error_free (err);
      g_free (debug);
      g_free (name);

      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_WARNING:{
      GError *err = NULL;
      gchar *name, *debug = NULL;

      name = gst_object_get_path_string (message->src);
      gst_message_parse_warning (message, &err, &debug);

      g_printerr ("ERROR: from element %s: %s\n", name, err->message);
      if (debug != NULL)
        g_printerr ("Additional debug info:\n%s\n", debug);

      g_error_free (err);
      g_free (debug);
      g_free (name);
      break;
    }
    case GST_MESSAGE_EOS:
      g_print ("Got EOS\n");
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }

  return TRUE;
}

int
main (int argc, char **argv)
{
  GstBus *bus;

  gst_init (&argc, &argv);

  pipeline = gst_pipeline_new (NULL);
  src = gst_element_factory_make ("videotestsrc", NULL);
  conv = gst_element_factory_make ("videoconvert", NULL);
  sink = gst_element_factory_make ("autovideosink", NULL);

  if (!pipeline || !src || !conv || !sink) {
    g_error ("Failed to create elements");
    return -1;
  }

  gst_bin_add_many (GST_BIN (pipeline), src, conv, sink, NULL);
  if (!gst_element_link_many (src, conv, sink, NULL)) {
    g_error ("Failed to link elements");
    return -2;
  }

  loop = g_main_loop_new (NULL, FALSE);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message", G_CALLBACK (message_cb), NULL);
  gst_object_unref (GST_OBJECT (bus));

  if (gst_element_set_state (pipeline,
          GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
    g_error ("Failed to go into PLAYING state");
    return -3;
  }

  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_main_loop_unref (loop);
  gst_object_unref (pipeline);

  return 0;
}
