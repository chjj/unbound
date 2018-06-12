#ifndef _NODE_UNBOUND_ASYNC_HH
#define _NODE_UNBOUND_ASYNC_HH

#include <node.h>
#include <nan.h>
#include <unbound.h>

class NodeUnboundWorker : public Nan::AsyncWorker {
public:
  NodeUnboundWorker (
    struct ub_ctx *ctx,
    char *name,
    int rrtype,
    int rrclass,
    Nan::Callback *callback
  );

  virtual ~NodeUnboundWorker();
  virtual void Execute();
  void HandleOKCallback();

private:
  struct ub_ctx *ctx;
  char *name;
  int rrtype;
  int rrclass;
  struct ub_result *result;
};

#endif
