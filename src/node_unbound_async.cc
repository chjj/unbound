#include <assert.h>
#include <unbound.h>

#include "node_unbound_async.h"

NodeUnboundWorker::NodeUnboundWorker (
  struct ub_ctx *ctx,
  char *name,
  int rrtype,
  int rrclass,
  Nan::Callback *callback
) : Nan::AsyncWorker(callback)
  , ctx(ctx)
  , name(name)
  , rrtype(rrtype)
  , rrclass(rrclass)
  , result(NULL)
{
  Nan::HandleScope scope;
}

NodeUnboundWorker::~NodeUnboundWorker() {
  if (name) {
    free(name);
    name = NULL;
  }

  if (result) {
    ub_resolve_free(result);
    result = NULL;
  }
}

void
NodeUnboundWorker::Execute() {
  int err = ub_resolve(ctx, name, rrtype, rrclass, &result);

  if (err != 0)
    SetErrorMessage(ub_strerror(err));
}

void
NodeUnboundWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  assert(result);

  uint8_t *pkt = (uint8_t *)result->answer_packet;
  size_t pkt_len = result->answer_len;

  v8::Local<v8::Array> ret = Nan::New<v8::Array>();

  ret->Set(0, Nan::CopyBuffer((char *)pkt, pkt_len).ToLocalChecked());
  ret->Set(1, Nan::New<v8::Boolean>((bool)result->secure));
  ret->Set(2, Nan::New<v8::Boolean>((bool)result->bogus));

  if (result->bogus && result->why_bogus)
    ret->Set(3, Nan::New<v8::String>(result->why_bogus).ToLocalChecked());
  else
    ret->Set(3, Nan::Null());

  ub_resolve_free(result);
  result = NULL;

  v8::Local<v8::Value> argv[] = { Nan::Null(), ret };

  callback->Call(2, argv, async_resource);
}
