#include <gst/gst.h>
#include <QObject>

#include <memory>

#ifndef __GST_QT_EXAMPLE_H__
#define __GST_QT_EXAMPLE_H__

class GstQtExample : public QObject
{
  Q_OBJECT

public:
  GstQtExample();
  virtual ~GstQtExample();

  void start(void);

private slots:
  void busMessage(std::shared_ptr<GstMessage> message);

private:
  static GstBusSyncReply busMessageDispatcher(GstBus *bus, GstMessage *message, gpointer user_data);

  std::shared_ptr<GstBus> bus;
  std::shared_ptr<GstElement> pipeline;
};

#endif /*__GST_QT_EXAMPLE_H__ */
