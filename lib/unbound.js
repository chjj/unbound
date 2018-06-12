/*!
 * unbound.js - unbound bindings for node.js
 * Copyright (c) 2018, Christopher Jeffrey (MIT License).
 * https://github.com/chjj/unbound
 */

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
    if (typeof opt === 'string')
      opt += ':';

    if (val == null)
      val = '';

    if (typeof val === 'boolean')
      val = val ? 'yes' : 'no';

    if (typeof val === 'number')
      val = val.toString(10);

    return super.setOption(opt, val);
  }

  getOption(opt, val) {
    if (typeof opt === 'string')
      opt += ':';

    const val = super.getOption(opt);

    if (val == null)
      return null;

    if (val === '')
      return null;

    if (val === 'yes')
      return true;

    if (val === 'no')
      return false;

    if (/^[0-9]+$/.test(val))
      return parseInt(val, 10);

    return val;
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
    return super.removeZone(name);
  }

  resolve(name, rrtype = 1, rrclass = 1) {
    this.finalized = true;
    return new Promise((resolve, reject) => {
      const cb = (err, items) => {
        if (err) {
          reject(err);
          return;
        }

        resolve(convert(items));
      };

      try {
        super.resolve(fqdn(name), rrtype, rrclass, cb);
      } catch (e) {
        reject(e);
      }
    });
  }
}

/*
 * Helpers
 */

function convert(items) {
  return {
    msg: items[0],
    secure: items[1],
    bogus: items[2],
    reason: items[3]
  };
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

/*
 * Expose
 */

module.exports = Unbound;
