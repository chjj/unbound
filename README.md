# unbound

Bindings to [libunbound] for node.js.

## Usage

``` js
const Unbound = require('unbound');
const ub = new Unbound();

const KSK_2017 = '. 172800 IN DS 20326 8 2'
  + ' E06D44B80B8F1D39A95C0B0D7C65D08458E880409BBC683457104237C7F8EC8D';

ub.addTrustAnchor(KSK_2017);

const result = await ub.resolve('www.ietf.org');
console.log(result);
```

Outputs:

``` js
UnboundResult {
  qname: 'www.ietf.org.',
  qtype: 1,
  qclass: 1,
  data: [ <Buffer 68 14 01 55>, <Buffer 68 14 00 55> ],
  canonName: 'www.ietf.org.cdn.cloudflare.net.',
  rcode: 0,
  answerPacket: <Buffer 00 00 81 a0 ... 474 more bytes>,
  haveData: true,
  nxDomain: false,
  secure: true,
  bogus: false,
  whyBogus: null,
  wasRateLimited: false,
  ttl: 300 }
```

## API

The API is a direct mapping to [libunbound calls][header].

- `Unbound.version()` - Return unbound version string.
- `Unbound#setOption(opt, val)` - Set [resolver option][conf]. Note that the
  trailing colon is not necessary. Values will be cast to strings (`null=''`,
  `bool='yes'/'no'`, `num=num.toString(10)`).
- `Unbound#getOption(opt)` - Get [resolver option][conf]. Note that the
  trailing colon is not necessary. Return values will be cast to bools,
  numbers, and nulls where appropriate.
- `Unbound#hasOption(opt)` - Test whether an option is available.
- `Unbound#tryOption(opt, val)` - Try to set option if available.
- `Unbound#setConfig(file)` - Read unbound config file.
- `Unbound#setForward(addr)` - Set host to forward DNS queries to.
- `Unbound#setStub(zone, addr, [prime=false])` - Setup stub zone.
- `Unbound#setResolvConf(file)` - Read from `resolv.conf`.
- `Unbound#setHosts(file)` - Read from an `/etc/hosts` file.
- `Unbound#addTrustAnchor(ta)` - Add a trust anchor (DS/DNSKEY presentation).
- `Unbound#addTrustAnchorFile(file, [autr=false])` - Add trust anchor file.
  Set `autr` for auto-updating and reading.
- `Unbound#addTrustedKeys(file)` - Add bind-style trust anchors.
- `Unbound#addZone(zoneName, zoneType)` - Add a zone to the local authority
  info.
- `Unbound#removeZone(zoneName)` - Remove zone.
- `Unbound#addData(data)` - Add localdata to the local authority info.
- `Unbound#removeData(data)` - Remove data.
- `Unbound#resolve(name, [type=A], [class=IN])` - Asynchronous recursive
  resolution (returns a `Promise`). `type` and `class` are raw qtypes and
  qclasses (no strings!). See above example for return value.

## Contribution and License Agreement

If you contribute code to this project, you are implicitly allowing your code
to be distributed under the MIT license. You are also implicitly verifying that
all code is your original work. `</legalese>`

## License

- Copyright (c) 2018-2020, Christopher Jeffrey (MIT License).

See LICENSE for more info.

[libunbound]: https://www.unbound.net/
[header]: https://github.com/NLnetLabs/unbound/blob/master/libunbound/unbound.h
[conf]: https://www.unbound.net/documentation/unbound.conf.html
