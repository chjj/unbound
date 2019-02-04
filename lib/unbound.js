/*!
 * unbound.js - unbound bindings for node.js
 * Copyright (c) 2018, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/unbound
 */

'use strict';

const {NodeUnbound} = require('bindings')('unbound');

/**
 * Unbound
 */

class Unbound extends NodeUnbound {
  constructor() {
    super();
    this.finalized = false;
  }

  setOption(opt, val) {
    if (val == null)
      val = '';

    if (typeof val === 'boolean')
      val = val ? 'yes' : 'no';

    if (typeof val === 'number')
      val = val.toString(10);

    return super.setOption(colon(opt), val);
  }

  getOption(opt) {
    const val = super.getOption(colon(opt));

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
      if (e.message.startsWith('libunbound:'))
        return false;
      throw e;
    }
  }

  tryOption(opt, val) {
    if (this.hasOption(opt))
      this.setOption(opt, val);
    return this;
  }

  setStub(zone, addr, prime = false) {
    return super.setStub(zone, addr, prime);
  }

  addTrustAnchorFile(file, autr = false) {
    return super.addTrustAnchorFile(file, autr);
  }

  addZone(name, type) {
    this.finalized = true;
    return super.addZone(name, type);
  }

  removeZone(name) {
    this.finalized = true;
    return super.removeZone(name);
  }

  addData(data) {
    this.finalized = true;
    return super.addData(data);
  }

  removeData(data) {
    this.finalized = true;
    return super.removeData(data);
  }

  resolve(qname, qtype = 1, qclass = 1) {
    this.finalized = true;
    return new Promise((resolve, reject) => {
      const cb = (err, items) => {
        if (err) {
          reject(err);
          return;
        }

        resolve(new UnboundResult(items));
      };

      try {
        super.resolve(fqdn(qname), qtype, qclass, cb);
      } catch (e) {
        reject(e);
      }
    });
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

  get msg() {
    return this.answerPacket;
  }

  set msg(val) {
    this.answerPacket = val;
  }

  get reason() {
    return this.whyBogus;
  }

  set reason(val) {
    this.whyBogus = val;
  }
}

/*
 * Helpers
 */

function colon(opt) {
  if (typeof opt !== 'string')
    return opt;

  if (opt.length === 0)
    return opt;

  if (opt[opt.length - 1] !== ':')
    return `${opt}:`;

  return opt;
}

function fqdn(name) {
  if (typeof name !== 'string')
    return name;

  if (name.length === 0)
    return '.';

  if (name[name.length - 1] !== '.')
    return `${name}.`;

  return name;
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
