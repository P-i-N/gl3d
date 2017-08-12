static const size_t monospace_base64_length = 5484;
static const char *monospace_base64 =
"gABAAONB4A4YeuHxDCAOR34i32czQTAYGGFjmAgwGyUzttlsE/IwGIBhZowZMBs1M7bZbPNaMBjfYGyMERAbFx+U2WybWjAY2GBsjDEYDggDnNls23occNhg"
"bIwxCD88PwzfZ9viMRjYYGzcIIw5LGMM2ADbojEY2GBseGCMMSpjDNhgu6ExGNhgbGBABH87PgfYYAv4MBiYYWZgwAYA~ETIDAYGWNiwAEAIIgHh+dj8yDgD"
"g564QGA5GzMhMdmZgMA~DCAh+RsbIxmbgyAAWxwcA8zzOjvTWTMZm8MgwFswBkMY2aAZM1mbOZtbIMBbIAZDGBmQGLNYuzvbGyDj++DGczDY+5vj8MGxmYGs"
"9ls9jwMg2FMYo7DA8bjA5vZbDYYDIPBTGIOA~CCP2WwGGAyDgQ8AAPh9ZjwPn9lsNhgMgwHA5+dgzGaGGZuZ7zMY3wcAzwyiYMxmgxkzA~EjMEMo2HMZoMZg"
"M9n5pkxOIzBB6FhfGYDH7MBbBbbOAy/x4wwYQxmAxieAWwGWzQGDN7M8GMMZgYMno/nB3swPgzYTBBjDDw8BwwYbIZ5MGYM2OcXAgA~BcGGzGWDBmjA8AAAC"
"fmWdmGhhsZtgwZjgA2L+gsdnMY7PHZ/ab/TwADMy2oLHZj0EzA~EnhnGtqSx2UBBgN8n+r5h+4wxw7a7sdlBMx7YLBqwYRsMYMY2Gx+fbwYzzKwasGEbjDHM"
"NhsA~BM8ys+rBhG4wZGAAA/h4AAB7GYxuwofoMDQDfzgA~B+wmYbsKEaDAFswNwA~BM8NmG7IhGt44bd8A~CDPBbPucLxoAdG0A~EeA~EQOQA~GM/u40Ufg6"
"EA~GB7YbTZsyI+xAQA~FD32A2bAzjtwEA~Fj9lgNijGjwEA~GLzZYDYoAoMBA~Hw32DmOR8A~JHwA~/A~/A~/A~/A~/A~WPDF8A~AgA~AbgAjAAMB/w4HIQA"
"AAH0A~ACCQMDByIAAABKACUABQMBAwcjA~ARQALAAcIAAQHJA~BsA~AGDAECByUAAABYA~ABwkAAwcmA~ATwA~AgJAAMHJwAAAFYAJAACAwMDBygA~AmA~AB"
"AwCAwcpA~AMA~BQMAQMHKgAAAC4AKQAGBQEDBysA~A1ACkABgUBBgcsA~AfAAcAAMFAQkHLQAAAGkAIwAEAQIIBy4AAABeACQAAgICCgcvA~ASQA~AUKAAMH"
"MA~AG0ACgAHCAAEBzEA~AqABcABggBBAcyA~AHAAXAAYIAQQHMwA~A4AFwAGCAEEBzQAAABlAAoABwgABAc1A~ABwAXAAYIAQQHNgAAADEAFwAGCAEEBzcA~"
"AHACAABggBBAc4A~DhAAYIAQQHOQAAAHcAEwAGCAAEBzoAAAB9AAoAAgYCBgc7A~APgAfAAMIAgYHPA~AEgAHQAFBwEFBz0AAABQACQABQMBBwc+A~AQgAeA"
"AUHAQUHPwAAABwADQAFCQIDB0A~BDA~ABwwAAwdBA~AVAAUAAYIAQQHQgAAAEYAFAAGCAEEB0MAAABwABMABggBBAdEA~AdQAKAAcIAQQHRQAAABsAIAAFCA"
"EEB0YA~AzACAABQgBBAdHA~APQAMAAcIAAQHSA~ABUAFwAGCAEEB0kA~A5AB8ABAgCBAdKA~AIQAgAAUIAQQHSwAAACMAFwAGCAEEB0wA~AnACAABQgBBAdN"
"A~ALQAgAAUIAQQHTgAAAE0ACwAHCAEEB08AAABdAAoABwgABAdQA~AYgATAAYIAQQHUQAAADoA~AHCwAEB1IA~AOACAABggBBAdTA~APwAVAAYIAQQHVA~AF"
"sAEwAGCAEEB1UAAABpABMABggBBAdWA~AVQAKAAcIAAQHVwAAABUAIAAFCAEEB1gA~CBgABggBBAdZA~ANAANAAgIAAQHWgAAAE0AFAAGCAEEB1sA~A1A~AB"
"AwBAwdcA~AQgA~AYKAQMHXQAAACsA~AEDAIDB14A~A8ACgABQQBBAdfA~AYQAjAAcBAA4HYA~AFkAJAAEAgEDB2EA~AHACkABgYBBgdiA~AFQANAAYJAQMHY"
"wAAABwAKQAFBgEGB2QA~AOAA0ABgkAAwdlA~AbgAcAAYGAQYHZgAAACIADQAFCQEDB2cAAABgA~ABwkABgdoA~ABwANAAYJAQMHaQAAAC4ADQAFCQEDB2oA~"
"AgA~ABQwBAwdrA~DOAAYJAQMHbA~ACgADQAFCQEDB20AAABOAB0ACAYABgduA~AFQApAAYGAQYHbwAAAF8AHAAHBgAGB3A~AB2A~ABgkBBgdxA~AbwA~AYJA"
"AYHcgA~A4AKQAGBgEGB3MA~CCoABgYBBgd0A~AOAAWAAYIAAQHdQAAAGcAHAAGBgEGB3YA~AiACkABQYBBgd3A~AVwAdAAcGAAYHeA~AHUAHAAGBgEGB3kAA"
"ABoA~ABgkABgd6A~AKAApAAUGAQYHewAAABIA~AGDAADB3wA~Gg0DAgd9A~AGQA~AYMAQMHfgAAAEIAJgAHAwAHBwA~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A"
"~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~/A~p=";
