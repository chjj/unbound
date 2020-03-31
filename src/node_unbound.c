/*!
 * node_unbound.c - unbound bindings for node.js
 * Copyright (c) 2018-2020, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/unbound
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <node_api.h>
#include <unbound.h>

#define CHECK(expr) do {                               \
  if (!(expr)) {                                       \
    fprintf(stderr, "%s:%d: Assertion `%s' failed.\n", \
            __FILE__, __LINE__, #expr);                \
    fflush(stderr);                                    \
    abort();                                           \
  }                                                    \
} while (0)

#define JS_THROW(msg) do {                              \
  CHECK(napi_throw_error(env, NULL, (msg)) == napi_ok); \
  return NULL;                                          \
} while (0)

#define JS_ASSERT(cond, msg) if (!(cond)) JS_THROW(msg)

#define JS_ERR_CONTEXT "Could not allocate context."
#define JS_ERR_STRING "String exceeds maximum length."

#define NODE_UB_MAX_STR (1024 + 1 + 1)
#define NODE_UB_MAX_ERR (12 + 256 + 2 + 11 + 1 + 1)

#define NODE_UB_THROW(err) do {                                 \
  char _msg[NODE_UB_MAX_ERR];                                   \
  node_ub_strerror(_msg, (err));                                \
  CHECK(napi_throw_error(env, "ERR_UNBOUND", _msg) == napi_ok); \
  return NULL;                                                  \
} while (0)

#define NODE_UB_CALL(call) do { \
  int _err = (call);            \
  if (_err != 0)                \
    NODE_UB_THROW(_err);        \
} while (0)

/*
 * Helpers
 */

static void
node_ub_strerror(char *out, int code) {
  const char *msg = ub_strerror(code);

  if (msg == NULL || strlen(msg) > 256)
    msg = "unknown error";

  sprintf(out, "libunbound: %s (%d)", msg, code);
}

/*
 * Unbound
 */

static napi_value
node_ub_version(napi_env env, napi_callback_info info) {
  napi_value result;

  CHECK(napi_create_string_utf8(env,
                                ub_version(),
                                NAPI_AUTO_LENGTH,
                                &result) == napi_ok);

  return result;
}

static void
node_ub_destroy(napi_env env, void *data, void *hint) {
  struct ub_ctx *ctx = data;

  ub_ctx_delete(ctx);
}

static napi_value
node_ub_create(napi_env env, napi_callback_info info) {
  struct ub_ctx *ctx = ub_ctx_create();
  napi_value handle;
  int err;

  JS_ASSERT(ctx != NULL, JS_ERR_CONTEXT);

  err = ub_ctx_debugout(ctx, NULL);

  if (err != 0) {
    ub_ctx_delete(ctx);
    NODE_UB_THROW(err);
  }

  err = ub_ctx_debuglevel(ctx, 0);

  if (err != 0) {
    ub_ctx_delete(ctx);
    NODE_UB_THROW(err);
  }

  CHECK(napi_create_external(env,
                             ctx,
                             node_ub_destroy,
                             NULL,
                             &handle) == napi_ok);

  return handle;
}

static napi_value
node_ub_set_option(napi_env env, napi_callback_info info) {
  napi_value argv[3];
  size_t argc = 3;
  char opt[NODE_UB_MAX_STR];
  char val[NODE_UB_MAX_STR];
  size_t opt_len, val_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 3);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], opt, sizeof(opt),
                                   &opt_len) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[2], val, sizeof(val),
                                   &val_len) == napi_ok);

  JS_ASSERT(opt_len != sizeof(opt) - 1, JS_ERR_STRING);
  JS_ASSERT(val_len != sizeof(val) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_set_option(ctx, opt, val));

  return argv[0];
}

static napi_value
node_ub_get_option(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char opt[NODE_UB_MAX_STR];
  size_t opt_len;
  char *val;
  struct ub_ctx *ctx;
  napi_value result;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], opt, sizeof(opt),
                                   &opt_len) == napi_ok);

  JS_ASSERT(opt_len != sizeof(opt) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_get_option(ctx, opt, &val));

  if (val == NULL) {
    CHECK(napi_get_null(env, &result) == napi_ok);
    return result;
  }

  CHECK(napi_create_string_utf8(env, val, NAPI_AUTO_LENGTH,
                                &result) == napi_ok);

  free(val);

  return result;
}

static napi_value
node_ub_set_config(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char fname[NODE_UB_MAX_STR];
  size_t fname_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], fname, sizeof(fname),
                                   &fname_len) == napi_ok);

  JS_ASSERT(fname_len != sizeof(fname) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_config(ctx, fname));

  return argv[0];
}

static napi_value
node_ub_set_forward(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char addr[NODE_UB_MAX_STR];
  size_t addr_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], addr, sizeof(addr),
                                   &addr_len) == napi_ok);

  JS_ASSERT(addr_len != sizeof(addr) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_set_fwd(ctx, addr));

  return argv[0];
}

static napi_value
node_ub_set_stub(napi_env env, napi_callback_info info) {
  napi_value argv[4];
  size_t argc = 4;
  char zone[NODE_UB_MAX_STR];
  char addr[NODE_UB_MAX_STR];
  size_t zone_len, addr_len;
  bool isprime;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 4);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], zone, sizeof(zone),
                                   &zone_len) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[2], addr, sizeof(addr),
                                   &addr_len) == napi_ok);
  CHECK(napi_get_value_bool(env, argv[3], &isprime) == napi_ok);

  JS_ASSERT(zone_len != sizeof(zone) - 1, JS_ERR_STRING);
  JS_ASSERT(addr_len != sizeof(addr) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_set_stub(ctx, zone, addr, isprime));

  return argv[0];
}

static napi_value
node_ub_set_resolvconf(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char fname[NODE_UB_MAX_STR];
  size_t fname_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], fname, sizeof(fname),
                                   &fname_len) == napi_ok);

  JS_ASSERT(fname_len != sizeof(fname) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_resolvconf(ctx, fname));

  return argv[0];
}

static napi_value
node_ub_set_hosts(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char fname[NODE_UB_MAX_STR];
  size_t fname_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], fname, sizeof(fname),
                                   &fname_len) == napi_ok);

  JS_ASSERT(fname_len != sizeof(fname) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_hosts(ctx, fname));

  return argv[0];
}

static napi_value
node_ub_add_ta(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char ta[NODE_UB_MAX_STR];
  size_t ta_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], ta, sizeof(ta),
                                   &ta_len) == napi_ok);

  JS_ASSERT(ta_len != sizeof(ta) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_add_ta(ctx, ta));

  return argv[0];
}

static napi_value
node_ub_add_ta_file(napi_env env, napi_callback_info info) {
  napi_value argv[3];
  size_t argc = 3;
  char fname[NODE_UB_MAX_STR];
  size_t fname_len;
  bool autr;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 3);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], fname, sizeof(fname),
                                   &fname_len) == napi_ok);
  CHECK(napi_get_value_bool(env, argv[2], &autr) == napi_ok);

  JS_ASSERT(fname_len != sizeof(fname) - 1, JS_ERR_STRING);

  if (autr)
    NODE_UB_CALL(ub_ctx_add_ta_autr(ctx, fname));
  else
    NODE_UB_CALL(ub_ctx_add_ta_file(ctx, fname));

  return argv[0];
}

static napi_value
node_ub_add_trustedkeys(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char fname[NODE_UB_MAX_STR];
  size_t fname_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], fname, sizeof(fname),
                                   &fname_len) == napi_ok);

  JS_ASSERT(fname_len != sizeof(fname) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_trustedkeys(ctx, fname));

  return argv[0];
}

static napi_value
node_ub_add_zone(napi_env env, napi_callback_info info) {
  napi_value argv[3];
  size_t argc = 3;
  char name[NODE_UB_MAX_STR];
  char type[NODE_UB_MAX_STR];
  size_t name_len, type_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 3);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], name, sizeof(name),
                                   &name_len) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[2], type, sizeof(type),
                                   &type_len) == napi_ok);

  JS_ASSERT(name_len != sizeof(name) - 1, JS_ERR_STRING);
  JS_ASSERT(type_len != sizeof(type) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_zone_add(ctx, name, type));

  return argv[0];
}

static napi_value
node_ub_remove_zone(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char name[NODE_UB_MAX_STR];
  size_t name_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], name, sizeof(name),
                                   &name_len) == napi_ok);

  JS_ASSERT(name_len != sizeof(name) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_zone_remove(ctx, name));

  return argv[0];
}

static napi_value
node_ub_add_data(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char data[NODE_UB_MAX_STR];
  size_t data_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], data, sizeof(data),
                                   &data_len) == napi_ok);

  JS_ASSERT(data_len != sizeof(data) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_data_add(ctx, data));

  return argv[0];
}

static napi_value
node_ub_remove_data(napi_env env, napi_callback_info info) {
  napi_value argv[2];
  size_t argc = 2;
  char data[NODE_UB_MAX_STR];
  size_t data_len;
  struct ub_ctx *ctx;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 2);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], data, sizeof(data),
                                   &data_len) == napi_ok);

  JS_ASSERT(data_len != sizeof(data) - 1, JS_ERR_STRING);

  NODE_UB_CALL(ub_ctx_data_remove(ctx, data));

  return argv[0];
}

typedef struct node_ub_worker_s {
  struct ub_ctx *ctx;
  char qname[NODE_UB_MAX_STR];
  uint32_t qtype;
  uint32_t qclass;
  struct ub_result *result;
  int error;
  napi_ref ref;
  napi_async_work work;
  napi_deferred deferred;
} node_ub_worker_t;

static void
node_ub_execute_(napi_env env, void *data) {
  node_ub_worker_t *w = data;

  w->error = ub_resolve(w->ctx, w->qname, w->qtype, w->qclass, &w->result);
}

static void
node_ub_complete_(napi_env env, napi_status status, void *data) {
  node_ub_worker_t *w = data;
  napi_value result;

  if (status != napi_ok)
    w->error = -8; /* UB_PIPE */

  if (w->error == 0) {
    struct ub_result *r = w->result;
    napi_value values[14];
    napi_value buf;
    size_t i;

    memset(values, 0, sizeof(values));

    if (r->qname != NULL) {
      CHECK(napi_create_string_utf8(env, r->qname, NAPI_AUTO_LENGTH,
                                    &values[0]) == napi_ok);
    } else {
      CHECK(napi_get_null(env, &values[0]) == napi_ok);
    }

    CHECK(napi_create_uint32(env, r->qtype, &values[1]) == napi_ok);
    CHECK(napi_create_uint32(env, r->qclass, &values[2]) == napi_ok);
    CHECK(napi_create_array(env, &values[3]) == napi_ok);

    if (r->data != NULL && r->len != NULL) {
      i = 0;

      while (r->data[i] != NULL) {
        CHECK(napi_create_buffer_copy(env, r->len[i], r->data[i],
                                      NULL, &buf) == napi_ok);
        CHECK(napi_set_element(env, values[3], i, buf) == napi_ok);
        i += 1;
      }
    }

    if (r->canonname != NULL) {
      CHECK(napi_create_string_utf8(env, r->canonname, NAPI_AUTO_LENGTH,
                                    &values[4]) == napi_ok);
    } else {
      CHECK(napi_get_null(env, &values[4]) == napi_ok);
    }

    CHECK(napi_create_uint32(env, r->rcode, &values[5]) == napi_ok);

    if (r->answer_len != 0) {
      CHECK(napi_create_buffer_copy(env, r->answer_len, r->answer_packet,
                                    NULL, &values[6]) == napi_ok);
    } else {
      CHECK(napi_get_null(env, &values[6]) == napi_ok);
    }

    CHECK(napi_get_boolean(env, r->havedata, &values[7]) == napi_ok);
    CHECK(napi_get_boolean(env, r->nxdomain, &values[8]) == napi_ok);
    CHECK(napi_get_boolean(env, r->secure, &values[9]) == napi_ok);
    CHECK(napi_get_boolean(env, r->bogus, &values[10]) == napi_ok);

    if (r->why_bogus != NULL) {
      CHECK(napi_create_string_utf8(env, r->why_bogus, NAPI_AUTO_LENGTH,
                                    &values[11]) == napi_ok);
    } else {
      CHECK(napi_get_null(env, &values[11]) == napi_ok);
    }

#if UNBOUND_VERSION_MAJOR > 1 \
|| (UNBOUND_VERSION_MAJOR == 1 && UNBOUND_VERSION_MINOR >= 8)
    CHECK(napi_get_boolean(env, r->was_ratelimited, &values[12]) == napi_ok);
#else
    CHECK(napi_get_boolean(env, false, &values[12]) == napi_ok);
#endif

    CHECK(napi_create_uint32(env, r->ttl, &values[13]) == napi_ok);

    CHECK(napi_create_array_with_length(env, 14, &result) == napi_ok);

    for (i = 0; i < 14; i++)
      CHECK(napi_set_element(env, result, i, values[i]) == napi_ok);

    CHECK(napi_resolve_deferred(env, w->deferred, result) == napi_ok);
  } else {
    char msg[NODE_UB_MAX_ERR];
    napi_value codeval, errval;

    node_ub_strerror(msg, w->error);

    CHECK(napi_create_string_utf8(env, "ERR_UNBOUND",
                                  NAPI_AUTO_LENGTH, &codeval) == napi_ok);
    CHECK(napi_create_string_utf8(env, msg,
                                  NAPI_AUTO_LENGTH, &errval) == napi_ok);

    CHECK(napi_create_error(env, codeval, errval, &result) == napi_ok);
    CHECK(napi_reject_deferred(env, w->deferred, result) == napi_ok);
  }

  CHECK(napi_delete_async_work(env, w->work) == napi_ok);
  CHECK(napi_delete_reference(env, w->ref) == napi_ok);

  if (w->result != NULL)
    ub_resolve_free(w->result);

  free(w);
}

static napi_value
node_ub_resolve(napi_env env, napi_callback_info info) {
  node_ub_worker_t *worker;
  napi_value argv[4];
  size_t argc = 4;
  char qname[NODE_UB_MAX_STR];
  size_t qname_len;
  uint32_t qtype, qclass;
  struct ub_ctx *ctx;
  napi_value workname, result;

  CHECK(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) == napi_ok);
  CHECK(argc == 4);
  CHECK(napi_get_value_external(env, argv[0], (void **)&ctx) == napi_ok);
  CHECK(napi_get_value_string_utf8(env, argv[1], qname, sizeof(qname),
                                   &qname_len) == napi_ok);
  CHECK(napi_get_value_uint32(env, argv[2], &qtype) == napi_ok);
  CHECK(napi_get_value_uint32(env, argv[3], &qclass) == napi_ok);

  JS_ASSERT(qname_len != sizeof(qname) - 1, JS_ERR_STRING);

  worker = malloc(sizeof(node_ub_worker_t));

  CHECK(worker != NULL);

  memset(worker, 0, sizeof(node_ub_worker_t));

  worker->ctx = ctx;
  memcpy(worker->qname, qname, qname_len + 1);
  worker->qtype = qtype;
  worker->qclass = qclass;
  worker->result = NULL;
  worker->error = 0;

  CHECK(napi_create_reference(env, argv[0], 1, &worker->ref) == napi_ok);

  CHECK(napi_create_string_utf8(env, "unbound:resolve",
                                NAPI_AUTO_LENGTH, &workname) == napi_ok);

  CHECK(napi_create_promise(env, &worker->deferred, &result) == napi_ok);

  CHECK(napi_create_async_work(env,
                               NULL,
                               workname,
                               node_ub_execute_,
                               node_ub_complete_,
                               worker,
                               &worker->work) == napi_ok);

  CHECK(napi_queue_async_work(env, worker->work) == napi_ok);

  return result;
}

/*
 * Module
 */

napi_value
node_ub_init(napi_env env, napi_value exports) {
  size_t i;

  static struct {
    const char *name;
    napi_callback callback;
  } funcs[] = {
    { "ub_version", node_ub_version },
    { "ub_create", node_ub_create },
    { "ub_set_option", node_ub_set_option },
    { "ub_get_option", node_ub_get_option },
    { "ub_set_config", node_ub_set_config },
    { "ub_set_forward", node_ub_set_forward },
    { "ub_set_stub", node_ub_set_stub },
    { "ub_set_resolvconf", node_ub_set_resolvconf },
    { "ub_set_hosts", node_ub_set_hosts },
    { "ub_add_ta", node_ub_add_ta },
    { "ub_add_ta_file", node_ub_add_ta_file },
    { "ub_add_trustedkeys", node_ub_add_trustedkeys },
    { "ub_add_zone", node_ub_add_zone },
    { "ub_remove_zone", node_ub_remove_zone },
    { "ub_add_data", node_ub_add_data },
    { "ub_remove_data", node_ub_remove_data },
    { "ub_resolve", node_ub_resolve }
  };

  for (i = 0; i < sizeof(funcs) / sizeof(funcs[0]); i++) {
    const char *name = funcs[i].name;
    napi_callback callback = funcs[i].callback;
    napi_value fn;

    CHECK(napi_create_function(env,
                               name,
                               NAPI_AUTO_LENGTH,
                               callback,
                               NULL,
                               &fn) == napi_ok);

    CHECK(napi_set_named_property(env, exports, name, fn) == napi_ok);
  }

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, node_ub_init)
