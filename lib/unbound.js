/*!
 * unbound.js - unbound bindings for node.js
 * Copyright (c) 2018-2020, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/unbound
 */

'use strict';

const binding = require('loady')('unbound', __dirname);

/**
 * Unbound
 */

class Unbound {
  constructor() {
    this._handle = binding.ub_create();
    this.finalized = false;
  }

  setOption(opt, val) {
    if (val == null)
      val = '';

    if (typeof val === 'boolean')
      val = val ? 'yes' : 'no';

    if (typeof val === 'number')
      val = val.toString(10);

    check(this instanceof Unbound);
    ensure(typeof opt === 'string', 'opt', 'string');
    ensure(typeof val === 'string', 'val', 'string');

    binding.ub_set_option(this._handle, colon(opt), val);

    return this;
  }

  getOption(opt) {
    check(this instanceof Unbound);
    ensure(typeof opt === 'string', 'opt', 'string');

    const val = binding.ub_get_option(this._handle, colon(opt));

    if (val == null)
      return null;

    if (val === '')
      return null;

    if (val === 'yes')
      return true;

    if (val === 'no')
      return false;

    if (isNumber(val))
      return parseInt(val, 10);

    return val;
  }

  hasOption(opt) {
    try {
      this.getOption(opt);
      return true;
    } catch (e) {
      if (e.code === 'ERR_UNBOUND')
        return false;
      throw e;
    }
  }

  tryOption(opt, val) {
    if (this.hasOption(opt))
      this.setOption(opt, val);
    return this;
  }

  setConfig(file) {
    check(this instanceof Unbound);
    ensure(typeof file === 'string', 'file', 'string');

    binding.ub_set_config(this._handle, file);

    return this;
  }

  setForward(addr) {
    check(this instanceof Unbound);
    ensure(typeof addr === 'string', 'addr', 'string');

    binding.ub_set_forward(this._handle, addr);

    return this;
  }

  setStub(zone, addr, prime = false) {
    check(this instanceof Unbound);
    ensure(typeof zone === 'string', 'zone', 'string');
    ensure(typeof addr === 'string', 'addr', 'string');
    ensure(typeof prime === 'boolean', 'prime', 'boolean');

    binding.ub_set_stub(this._handle, zone, addr, prime);

    return this;
  }

  setResolveConf(file) {
    check(this instanceof Unbound);
    ensure(typeof file === 'string', 'file', 'string');

    binding.ub_set_resolvconf(this._handle, file);

    return this;
  }

  setHosts(file) {
    check(this instanceof Unbound);
    ensure(typeof file === 'string', 'file', 'string');

    binding.ub_set_hosts(this._handle, file);

    return this;
  }

  addTrustAnchor(ta) {
    check(this instanceof Unbound);
    ensure(typeof ta === 'string', 'ta', 'string');

    binding.ub_add_ta(this._handle, ta);

    return this;
  }

  addTrustAnchorFile(file, autr = false) {
    check(this instanceof Unbound);
    ensure(typeof file === 'string', 'file', 'string');
    ensure(typeof autr === 'boolean', 'autr', 'boolean');

    binding.ub_add_ta_file(this._handle, file, autr);

    return this;
  }

  addTrustedKeys(file) {
    check(this instanceof Unbound);
    ensure(typeof file === 'string', 'file', 'string');

    binding.ub_add_trustedkeys(this._handle, file);

    return this;
  }

  addZone(name, type) {
    check(this instanceof Unbound);
    ensure(typeof name === 'string', 'name', 'string');
    ensure(typeof type === 'string', 'type', 'string');

    binding.ub_add_zone(this._handle, name, type);

    this.finalized = true;

    return this;
  }

  removeZone(name) {
    check(this instanceof Unbound);
    ensure(typeof name === 'string', 'name', 'string');

    binding.ub_remove_zone(this._handle, name);

    this.finalized = true;

    return this;
  }

  addData(data) {
    check(this instanceof Unbound);
    ensure(typeof data === 'string', 'data', 'string');

    binding.ub_add_data(this._handle, data);

    this.finalized = true;

    return this;
  }

  removeData(data) {
    check(this instanceof Unbound);
    ensure(typeof data === 'string', 'data', 'string');

    binding.ub_remove_data(this._handle, data);

    this.finalized = true;

    return this;
  }

  async resolve(qname, qtype = 1, qclass = 1) {
    check(this instanceof Unbound);
    ensure(typeof qname === 'string', 'qname', 'string');
    ensure((qtype >>> 0) === qtype, 'qtype', 'uint32');
    ensure((qclass >>> 0) === qclass, 'qclass', 'uint32');

    const promise = binding.ub_resolve(this._handle,
                                       fqdn(qname),
                                       qtype,
                                       qclass);

    this.finalized = true;

    return new UnboundResult(await promise);
  }

  static version() {
    return binding.ub_version();
  }
}

/**
 * UnboundResult
 */

class UnboundResult {
  constructor(items) {
    this.qname = items[0];
    this.qtype = items[1];
    this.qclass = items[2];
    this.data = items[3];
    this.canonName = items[4];
    this.rcode = items[5];
    this.answerPacket = items[6];
    this.haveData = items[7];
    this.nxDomain = items[8];
    this.secure = items[9];
    this.bogus = items[10];
    this.whyBogus = items[11];
    this.wasRateLimited = items[12];
    this.ttl = items[13];
  }
}

/*
 * Helpers
 */

function check(ok) {
  if (!ok)
    throw new TypeError('Not an Unbound instance.');
}

function ensure(ok, name, type) {
  if (!ok)
    throw new TypeError(`"${name}" must be a ${type}.`);
}

function colon(opt) {
  if (typeof opt !== 'string')
    return opt;

  if (opt.length === 0)
    return opt;

  if (opt[opt.length - 1] !== ':')
    return `${opt}:`;

  return opt;
}

function isFQDN(s) {
  if (s.length === 0)
    return false;

  const i = s.length - 1;

  if (s.charCodeAt(i) !== 0x2e)
    return false;

  let j = i - 1;

  while (j >= 0 && s.charCodeAt(j) === 0x5c)
    j -= 1;

  return (j - i) % 2 !== 0;
}

function fqdn(s) {
  if (typeof s !== 'string')
    return s;

  if (isFQDN(s))
    return s;

  return s + '.';
}

function isNumber(str) {
  if (typeof str !== 'string')
    return false;

  if (str.length < 1 || str.length > 15)
    return false;

  return /^[0-9]{1,15}$/.test(str);
}

/*
 * Expose
 */

module.exports = Unbound;
