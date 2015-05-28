#include <QMetaType>
#include <QCoreApplication>

#include <memory>

#include "gst-qt-example.h"

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr);
Q_DECLARE_METATYPE(std::shared_ptr<GstMessage>);

template <typename T>
std::shared_ptr<T>
takeGstObject(T *o)
{
  std::shared_ptr<T> ptr(o, [] (T *d) {
    gst_object_unref(reinterpret_cast<GstObject*>(d));
  });

  return ptr;
}

template <typename T>
std::shared_ptr<T>
takeGstMiniObject(T *o)
{
  std::shared_ptr<T> ptr(o, [] (T *d) {
    gst_mini_object_unref(reinterpret_cast<GstMiniObject*>(d));
  });

  return ptr;
}

GstQtExample::GstQtExample()
{
  pipeline = takeGstObject(gst_parse_launch("audiotestsrc num-buffers=100 ! autoaudiosink", nullptr));
  bus = takeGstObject(gst_element_get_bus(pipeline.get()));
  gst_bus_set_sync_handler(bus.get(), busMessageDispatcher, this, nullptr);
}

GstQtExample::~GstQtExample()
{
  gst_bus_set_sync_handler(bus.get(), nullptr, nullptr, nullptr);
}

void
GstQtExample::busMessage(std::shared_ptr<GstMessage> message)
{
  switch (GST_MESSAGE_TYPE(message.get())) {
    case GST_MESSAGE_STATE_CHANGED:
      if (GST_MESSAGE_SRC(message.get()) == reinterpret_cast<GstObject*>(pipeline.get())) {
        GstState newState;

        gst_message_parse_state_changed(message.get(), nullptr, &newState, nullptr);
        if (newState == GST_STATE_PLAYING) {
          g_print("reached PLAYING\n");
        }
      }
      break;
    case GST_MESSAGE_EOS:
      g_print("reached EOS\n");
      gst_element_set_state(pipeline.get(), GST_STATE_NULL);
      QCoreApplication::exit();
      break;
    default:
      break;
  }
}

GstBusSyncReply
GstQtExample::busMessageDispatcher(GstBus *bus, GstMessage *message, gpointer userData)
{
  auto example = static_cast<GstQtExample*>(userData);

  Q_UNUSED(bus);

  auto messagePtr = takeGstMiniObject(message);
  QMetaObject::invokeMethod(example, "busMessage", Qt::QueuedConnection, Q_ARG(std::shared_ptr<GstMessage>, messagePtr));

  return GST_BUS_DROP;
}

void
GstQtExample::start(void)
{
  gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}

int
main(int argc, char **argv)
{
  gst_init(&argc, &argv);
  QCoreApplication app(argc, argv);

  // Register our shared pointer type
  QMetaTypeId<std::shared_ptr<GstMessage>>::qt_metatype_id();

  GstQtExample example;
  example.start();

  return app.exec();
}
