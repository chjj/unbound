/* eslint-env mocha */
/* eslint prefer-arrow-callback: "off" */

'use strict';

const assert = require('assert');
const Unbound = require('../');

const KSK_2017 = '. 172800 IN DS 20326 8 2'
  + ' E06D44B80B8F1D39A95C0B0D7C65D08458E880409BBC683457104237C7F8EC8D';

describe('Unbound', function() {
  this.timeout(10000);

  it('should resolve www.ietf.org.', async () => {
    const ub = new Unbound();

    assert(ub.hasOption('logfile'));
    assert(!ub.hasOption('foobar'));
    ub.tryOption('foobar', false);

    ub.setOption('logfile', null);
    ub.setOption('use-syslog', false);
    ub.tryOption('trust-anchor-signaling', false);
    ub.setOption('edns-buffer-size', 4096);
    ub.setOption('max-udp-size', 4096);
    ub.setOption('msg-cache-size', 4 << 20);
    ub.setOption('key-cache-size', 4 << 20);
    ub.setOption('neg-cache-size', 4 << 20);
    ub.setOption('do-ip4', true);
    ub.setOption('do-ip6', false);
    ub.setOption('do-udp', true);
    ub.setOption('do-tcp', false);
    ub.tryOption('qname-minimisation', true);
    ub.setOption('minimal-responses', false);

    assert.strictEqual(ub.getOption('logfile'), null);
    assert.strictEqual(ub.getOption('qname-minimisation'), true);
    assert.strictEqual(ub.getOption('max-udp-size'), 4096);

    ub.addTrustAnchor(KSK_2017);

    const res = await ub.resolve('www.ietf.org');

    assert(res && typeof res === 'object');
    assert.strictEqual(res.qname, 'www.ietf.org.');
    assert.strictEqual(res.qtype, 1);
    assert.strictEqual(res.qclass, 1);
    assert(Array.isArray(res.data));
    assert(res.data.length > 0);
    assert(Buffer.isBuffer(res.data[0]));
    assert.strictEqual(res.canonName, 'www.ietf.org.cdn.cloudflare.net.');
    assert.strictEqual(res.rcode, 0);
    assert(Buffer.isBuffer(res.answerPacket));
    assert(res.answerPacket.length > 0);
    assert.strictEqual(res.haveData, true);
    assert.strictEqual(res.nxDomain, false);
    assert.strictEqual(res.secure, true);
    assert.strictEqual(res.bogus, false);
    assert.strictEqual(res.whyBogus, null);
    assert.strictEqual(res.wasRateLimited, false);
    assert.strictEqual(res.ttl, 300);
  });
});
