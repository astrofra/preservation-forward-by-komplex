#include "audio/module_player.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace forward_offline {

namespace {

// This translation unit intentionally stays close to the original Java
// replayer structure to reduce behavioral drift while keeping the API small.

struct EnvelopePoint {
    int x;
    int y;
};

struct Envelope {
    std::vector<int> MaJAKKa;
    int maJAKKa;
    int MAJAKKa;
    int mAJAKKa;
    int MajAKKa;
    bool majAKKa;
    bool MAjAKKa;
    bool mAjAKKa;

    Envelope()
        : MaJAKKa(),
          maJAKKa(0),
          MAJAKKa(0),
          mAJAKKa(0),
          MajAKKa(0),
          majAKKa(false),
          MAjAKKa(false),
          mAjAKKa(false) {
    }

    Envelope(const std::vector<EnvelopePoint>& point_array, int n, int n2, int n3)
        : MaJAKKa(),
          maJAKKa(0),
          MAJAKKa(0),
          mAJAKKa(0),
          MajAKKa(0),
          majAKKa(false),
          MAjAKKa(false),
          mAjAKKa(false) {
        if (n >= 0 && n < static_cast<int>(point_array.size())) {
            maJAKKa = point_array[static_cast<std::size_t>(n)].x;
        }
        if (n2 >= 0 && n2 < static_cast<int>(point_array.size())) {
            MAJAKKa = point_array[static_cast<std::size_t>(n2)].x;
        }
        if (n3 >= 0 && n3 < static_cast<int>(point_array.size())) {
            mAJAKKa = point_array[static_cast<std::size_t>(n3)].x;
        }
        if (point_array.empty()) {
            return;
        }

        MaJAKKa.assign(static_cast<std::size_t>(point_array.back().x + 1), 0);
        int current_x = 0;
        for (std::size_t index = 0; index + 1 < point_array.size(); ++index) {
            const int x0 = point_array[index].x;
            const int y0 = point_array[index].y;
            const int x1 = point_array[index + 1].x;
            const int y1 = point_array[index + 1].y;
            int value = y0 << 8;
            const int step = x1 == x0 ? 0 : ((y1 << 8) - (y0 << 8)) / (x1 - x0);
            current_x = x0;
            while (current_x < x1 &&
                   current_x < static_cast<int>(MaJAKKa.size())) {
                MaJAKKa[static_cast<std::size_t>(current_x)] = value >> 8;
                value += step;
                ++current_x;
            }
        }

        if (!MaJAKKa.empty()) {
            MaJAKKa[static_cast<std::size_t>(current_x)] = point_array.back().y;
            majAKKa = true;
        }
    }

    void amAjakK(int n) {
        MajAKKa = n;
    }

    int AmAjakK(int n) const {
        if (MaJAKKa.empty()) {
            return 0;
        }
        if (n >= static_cast<int>(MaJAKKa.size())) {
            return MaJAKKa.back();
        }
        return MaJAKKa[static_cast<std::size_t>(n)];
    }

    int AMAjakK(int n, bool bl) const {
        ++n;
        if (bl) {
            if (MAjAKKa && n != maJAKKa && n >= mAJAKKa) {
                n = MAJAKKa;
            }
        } else {
            if (mAjAKKa && n >= maJAKKa) {
                n = maJAKKa;
            }
            if (MAjAKKa && n >= mAJAKKa) {
                n = MAJAKKa;
            }
        }
        if (!MaJAKKa.empty() && n >= static_cast<int>(MaJAKKa.size())) {
            n = static_cast<int>(MaJAKKa.size()) - 1;
        }
        return n;
    }
};

struct EnvelopeCursor {
    const Envelope* jaKkAMA;
    int JAKkAMA;
    bool jAKkAMA;
    bool JakkAMA;
    int jakkAMA;

    EnvelopeCursor()
        : jaKkAMA(NULL),
          JAKkAMA(0),
          jAKkAMA(false),
          JakkAMA(false),
          jakkAMA(0) {
    }

    void KKAMAja(const Envelope* kajamma2) {
        if (kajamma2 == NULL || !kajamma2->majAKKa) {
            JakkAMA = false;
            jaKkAMA = NULL;
            kkAMAja();
            return;
        }
        JakkAMA = true;
        jaKkAMA = kajamma2;
        kkAMAja();
    }

    void kkAMAja() {
        JAKkAMA = 0;
        jAKkAMA = false;
    }

    void KkAMAja() {
        jAKkAMA = true;
        jakkAMA = 32768;
    }

    int kKAMAja() {
        if (jaKkAMA == NULL) {
            return 0;
        }
        const int value = jaKkAMA->AmAjakK(JAKkAMA);
        JAKkAMA = jaKkAMA->AMAjakK(JAKkAMA, jAKkAMA);
        if (jAKkAMA) {
            jakkAMA -= jaKkAMA->MajAKKa;
            if (jakkAMA <= 0) {
                jakkAMA = 0;
                return 0;
            }
            return value * jakkAMA >> 15;
        }
        return value;
    }
};

struct ModuleSample;

struct ModuleInstrument {
    std::vector<ModuleSample*> AkkamAJ;
    Envelope akkamAJ;
    Envelope AKkamAJ;
    int aKkamAJ;
    int AkKAmAJ;
    int akKAmAJ;
    int AKKAmAJ;

    ModuleInstrument()
        : AkkamAJ(96, NULL),
          akkamAJ(),
          AKkamAJ(),
          aKkamAJ(0),
          AkKAmAJ(0),
          akKAmAJ(0),
          AKKAmAJ(0) {
    }

    void routineRegistry(ModuleSample* mmajmka2) {
        for (std::size_t index = 0; index < AkkamAJ.size(); ++index) {
            AkkamAJ[index] = mmajmka2;
        }
    }
};

const std::vector<int>& module_channel_frequency_table() {
    static std::vector<int> table;
    if (!table.empty()) {
        return table;
    }

    table.resize(768, 0);
    for (int index = 0; index < 768; ++index) {
        table[static_cast<std::size_t>(index)] = static_cast<int>(
            8363.0 * std::pow(2.0, (4608.0 - static_cast<double>(index)) / 768.0));
    }
    return table;
}

const std::vector<int>& module_channel_vibrato_table() {
    static std::vector<int> table;
    if (!table.empty()) {
        return table;
    }

    table.resize(32, 0);
    for (int index = 0; index < 32; ++index) {
        table[static_cast<std::size_t>(index)] = static_cast<int>(
            std::sin(static_cast<double>(index) / 32.0 * 3.14159265358979323846) * 255.0);
    }
    return table;
}

const std::vector<int>& module_channel_wave_table() {
    static std::vector<int> table;
    if (!table.empty()) {
        return table;
    }

    table.resize(256, 0);
    for (int index = 0; index < 256; ++index) {
        table[static_cast<std::size_t>(index)] = static_cast<int>(
            -std::sin(static_cast<double>(index) / 256.0 * 3.14159265358979323846 * 2.0) *
            64.55);
    }
    return table;
}

struct ModuleSample {
    std::string kAmajaK;
    std::vector<std::int8_t> KaMAjaK;
    int kaMAjaK;
    int KAMAjaK;
    int kAMAjaK;
    int KamAjaK;
    int kamAjaK;
    int KAmAjaK;
    int kAmAjaK;
    int KaMaJAK;
    int kaMaJAK;

    ModuleSample()
        : kAmajaK("unnamed"),
          KaMAjaK(),
          kaMAjaK(0),
          KAMAjaK(32),
          kAMAjaK(0),
          KamAjaK(0),
          kamAjaK(0),
          KAmAjaK(0),
          kAmAjaK(0),
          KaMaJAK(0),
          kaMaJAK(128) {
    }

    int KamajAK(int n) const {
        n += KAMAjaK;
        if (--n < 0) {
            n = 0;
        }
        if (n > 118) {
            n = 118;
        }
        return 7680 - n * 16 * 4 - kAMAjaK / 2;
    }

    static int kAmajAK(int n) {
        if (n < 0) {
            return 1111111;
        }
        const std::vector<int>& table = module_channel_frequency_table();
        const int octave = n / 768;
        const int frequency = table[static_cast<std::size_t>(n % 768)] >> octave;
        return frequency << 12;
    }

    void kAMajAK(const std::vector<std::int8_t>& by_array) {
        KaMAjaK = by_array;
        kaMAjaK = static_cast<int>(by_array.size());
    }

    void kAmAJAK(const std::string& string) {
        kAmajaK = string;
    }

    void kaMajAK(int n, int n2, int n3) {
        kamAjaK = n;
        KAmAjaK = n2 << 12;
        KaMaJAK = n3 << 12;
        kAmAjaK = KAmAjaK + KaMaJAK;
    }

    void KAmAJAK(int n) {
        KAMAjaK = n;
    }

    void KAMajAK(int n) {
        kAMAjaK = n;
    }

    void KaMajAK(int n) {
        KamAjaK = n;
    }

    void KAmajAK(int n) {
        kaMaJAK = n;
    }

    int kamajAK(std::vector<std::uint32_t>* n_array,
                int n,
                int n2,
                int n3,
                int n4,
                int n5,
                int n6) const {
        int n7 = 255 - n6 + (n6 << 16);
        n7 = n7 * n5 >> 8;
        n7 &= 0xFF00FF;

        switch (kamAjaK) {
            case 0: {
                const int n8 = kaMAjaK << 12;
                int n9 = (n8 - 1 - n3) / n4 + 1;
                if (n9 > n2 - n) {
                    n9 = n2 - n;
                }
                switch (n9 & 3) {
                    case 3:
                        (*n_array)[static_cast<std::size_t>(n++)] +=
                            static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                        n3 += n4;
                    case 2:
                        (*n_array)[static_cast<std::size_t>(n++)] +=
                            static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                        n3 += n4;
                    case 1:
                        (*n_array)[static_cast<std::size_t>(n++)] +=
                            static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                        n3 += n4;
                    case 0:
                        n9 >>= 2;
                        while (n9-- > 0) {
                            (*n_array)[static_cast<std::size_t>(n++)] +=
                                static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                            (*n_array)[static_cast<std::size_t>(n++)] +=
                                static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                            (*n_array)[static_cast<std::size_t>(n++)] +=
                                static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                            (*n_array)[static_cast<std::size_t>(n++)] +=
                                static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                            n3 += n4;
                        }
                        break;
                }
                if (n3 >= n8) {
                    n3 = -1;
                }
                break;
            }
            case 1: {
                const int n17 = kAmAjaK;
                int n18 = (n17 - 1 - n3) / n4 + 1;
                while (n < n2) {
                    if (n18 > n2 - n) {
                        n18 = n2 - n;
                    }
                    switch (n18 & 3) {
                        case 3:
                            (*n_array)[static_cast<std::size_t>(n++)] +=
                                static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                            n3 += n4;
                        case 2:
                            (*n_array)[static_cast<std::size_t>(n++)] +=
                                static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                            n3 += n4;
                        case 1:
                            (*n_array)[static_cast<std::size_t>(n++)] +=
                                static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                            n3 += n4;
                        case 0:
                            n18 >>= 2;
                            while (n18-- > 0) {
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                                n3 += n4;
                            }
                            break;
                    }
                    if (n3 < n17) {
                        continue;
                    }
                    n3 = KAmAjaK + (n3 - KAmAjaK) % KaMaJAK;
                    n18 = (n17 - 1 - n3) / n4 + 1;
                }
                break;
            }
            case 2: {
                int n26 = n3 >= kAmAjaK ? kAmAjaK + KaMaJAK : kAmAjaK;
                int n27 = (n26 - 1 - n3) / n4 + 1;
                while (n < n2) {
                    if (n27 > n2 - n) {
                        n27 = n2 - n;
                    }
                    if (n3 >= kAmAjaK) {
                        n3 = kAmAjaK + (kAmAjaK - n3) - 1;
                        switch (n27 & 3) {
                            case 3:
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                n3 -= n4;
                            case 2:
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                n3 -= n4;
                            case 1:
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                n3 -= n4;
                            case 0:
                                n27 >>= 2;
                                while (n27-- > 0) {
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 -= n4) >> 12)] * n7);
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 -= n4) >> 12)] * n7);
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 -= n4) >> 12)] * n7);
                                    n3 -= n4;
                                }
                                break;
                        }
                        n3 = kAmAjaK + (kAmAjaK - n3) + 1;
                    } else {
                        switch (n27 & 3) {
                            case 3:
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                n3 += n4;
                            case 2:
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                n3 += n4;
                            case 1:
                                (*n_array)[static_cast<std::size_t>(n++)] +=
                                    static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                n3 += n4;
                            case 0:
                                n27 >>= 2;
                                while (n27-- > 0) {
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>(n3 >> 12)] * n7);
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                                    (*n_array)[static_cast<std::size_t>(n++)] +=
                                        static_cast<std::uint32_t>(KaMAjaK[static_cast<std::size_t>((n3 += n4) >> 12)] * n7);
                                    n3 += n4;
                                }
                                break;
                        }
                    }
                    if (n3 < n26) {
                        continue;
                    }
                    n3 = KAmAjaK + (n3 - KAmAjaK) % (KaMaJAK << 1);
                    if (n3 >= kAmAjaK) {
                        n26 = kAmAjaK + KaMaJAK;
                        n27 = (n26 - 1 - n3) / n4 + 1;
                    } else {
                        n26 = kAmAjaK;
                        n27 = (n26 - 1 - n3) / n4 + 1;
                    }
                }
                break;
            }
        }
        return n3;
    }
};

struct OfflineMad {
    int frequency;
    bool stereo;
    int boost;

    OfflineMad()
        : frequency(22050),
          stereo(true),
          boost(128) {
    }
};

struct ModuleChannel {
    ModuleSample* kKamAja;
    int KkAMAja;
    int kkAMAja;
    int KKAMAja;
    int kKAMAja;
    int KkaMAja;
    int kkaMAja;
    int KKaMAja;
    int kKaMAja;
    int KkAmaja;
    bool kkAmaja;
    std::uint8_t kkaMaja;
    int KKaMaja;
    int kKaMaja;
    int KkAmAJa;
    int kkAmAJa;
    int KKAmAJa;
    int kKAmAJa;
    int KkamAJa;
    int kkamAJa[3];
    int KKamAJa;
    int kKamAJa;
    int KkAMAJa;
    int kkAMAJa;
    int KKAMAJa;
    int kKAMAJa;
    int KkaMAJa;
    int kkaMAJa;
    int KKaMAJa;
    int kKaMAJa;
    int KkAmaJa;
    int kkAmaJa;
    int KKAmaJa;

    ModuleChannel()
        : kKamAja(NULL),
          KkAMAja(64),
          kkAMAja(64),
          KKAMAja(128),
          kKAMAja(128),
          KkaMAja(0),
          kkaMAja(-1),
          KKaMAja(0),
          kKaMAja(0),
          KkAmaja(0),
          kkAmaja(true),
          kkaMaja(0),
          KKaMaja(0),
          kKaMaja(0),
          KkAmAJa(0),
          kkAmAJa(0),
          KKAmAJa(0),
          kKAmAJa(0),
          KkamAJa(0),
          kkamAJa{0, 0, 0},
          KKamAJa(0),
          kKamAJa(0),
          KkAMAJa(0),
          kkAMAJa(0),
          KKAMAJa(0),
          kKAMAJa(0),
          KkaMAJa(0),
          kkaMAJa(0),
          KKaMAJa(0),
          kKaMAJa(0),
          KkAmaJa(0),
          kkAmaJa(0),
          KKAmaJa(0) {
    }

    void kAMAjAk(bool bl) {
        kkAmaja = bl;
    }

    void KaMAjAk(int n) {
        KkAMAja = n <= 0 ? 0 : (n > 64 ? 64 : n);
        kkAMAja = KkAMAja;
    }

    void kamAjAk(int n) {
        const int n2 = KkAMAja + n;
        kkAMAja = n2 <= 0 ? 0 : (n2 > 64 ? 64 : n2);
    }

    void kAmAjAk(int n) {
        const int n2 = kkAMAja + n;
        kkAMAja = n2 <= 0 ? 0 : (n2 > 64 ? 64 : n2);
    }

    void KKAmAjA(int n) {
        kkAMAja = n <= 0 ? 0 : (n > 64 ? 64 : n);
    }

    void KamAjAk(int n) {
        kKaMAja = KKaMAja = n;
        KkAmaja = ModuleSample::kAmajAK(kKaMAja);
    }

    void kaMajAk(int n) {
        kKaMAja = KKaMAja + n;
        KkAmaja = ModuleSample::kAmajAK(kKaMAja);
    }

    void KAMajAk(int n) {
        kKaMAja += n;
        KkAmaja = ModuleSample::kAmajAK(kKaMAja);
    }

    void kAmajAk(int n) {
        KKAMAja = n <= 0 ? 0 : (n > 255 ? 255 : n);
        kKAMAja = KKAMAja;
    }

    void kkAmAjA(int n) {
        kKAMAja = n <= 0 ? 0 : (n > 255 ? 255 : n);
    }

    void kaMAjAk(int n) {
        const int n2 = KKAMAja + n;
        kKAMAja = n2 <= 0 ? 0 : (n2 > 255 ? 255 : n2);
    }

    void KAMAjAk(ModuleSample* mmajmka2, int n) {
        KkAmAjA(mmajmka2, n, true);
    }

    void KkAmAjA(ModuleSample* mmajmka2, int n, bool bl) {
        if (mmajmka2 == NULL) {
            KAmAjAk();
            return;
        }
        kKamAja = mmajmka2;
        KamAjAk(kKamAja->KamajAK(n));
        if (bl) {
            KaMAjAk(kKamAja->KamAjaK);
        }
        kkaMAja = 0;
        KKAMAja = kKamAja->kaMaJAK;
    }

    void KaMajAk() {
        KAmajAk(true);
    }

    void KAmajAk(bool bl) {
        if (kKamAja == NULL) {
            return;
        }
        if (bl) {
            KaMAjAk(kKamAja->KamAjaK);
        }
        kkaMAja = 0;
        KKAMAja = kKamAja->kaMaJAK;
    }

    void kAMajAk(int n, bool bl) {
        if (kKamAja == NULL) {
            return;
        }
        KamAjAk(kKamAja->KamajAK(n));
        if (bl) {
            KaMAjAk(kKamAja->KamAjaK);
        }
        kkaMAja = 0;
        KKAMAja = kKamAja->kaMaJAK;
    }

    void kamajAk() {
        if (kKamAja == NULL) {
            return;
        }
        KaMAjAk(kKamAja->KamAjaK);
    }

    void KAmAjAk() {
        kkaMAja = -1;
    }

    void KamajAk(int n) {
        if (kKamAja == NULL) {
            return;
        }
        if (n >= kKamAja->kaMAjaK) {
            kkaMAja = -1;
            return;
        }
        kkaMAja = n << 12;
    }

    void mix(const OfflineMad& mAD, std::vector<std::uint32_t>* n_array, int n, int n2) {
        if (kkaMAja == -1 || !kkAmaja || kKamAja == NULL || kKamAja->KaMAjaK.empty()) {
            return;
        }
        const int n3 = kkAMAja * mAD.boost >> 6;
        const int n4 = KkAmaja / mAD.frequency;
        if (n4 <= 0) {
            return;
        }
        kkaMAja = mAD.stereo
            ? kKamAja->kamajAK(n_array, n, n2, kkaMAja, n4, n3, KKAMAja)
            : kKamAja->kamajAK(n_array, n, n2, kkaMAja, n4, n3, 0);
    }

    void kAmAJAk(bool bl) {
        int n = 0;
        int n2 = 0;
        int n3 = 0;
        if (bl) {
            int n4 = 0;
            if (kkaMaja == 5) {
                n4 = module_channel_vibrato_table()[static_cast<std::size_t>(kKaMAJa & 0x1F)] *
                     kkaMAJa >> 5;
                if ((kKaMAJa & 0x20) != 0) {
                    n4 = -n4;
                }
                n2 = n4;
                kKaMAJa += KKaMAJa;
            }
            if (kkaMaja == 11) {
                n4 = module_channel_vibrato_table()[static_cast<std::size_t>(KKAmaJa & 0x1F)] *
                     KkAmaJa >> 6;
                if ((KKAmaJa & 0x20) != 0) {
                    n4 = -n4;
                }
                n = n4;
                KKAmaJa += kkAmaJa;
            }
            kkaMaja = 0;
        } else {
            switch (kkaMaja) {
                case 10:
                    KaMAjAk(KkAMAja + KKaMaja);
                case 2:
                    KKaMAja += KkAmAJa;
                    if ((KkAmAJa < 0) ^ (KKaMAja > kKAmAJa)) {
                        KamAjAk(kKAmAJa);
                    } else {
                        KamAjAk(KKaMAja);
                    }
                    break;
                case 3:
                    KKaMAja += kkAmAJa;
                    KamAjAk(KKaMAja);
                    break;
                case 4:
                    KKaMAja += KKAmAJa;
                    KamAjAk(KKaMAja);
                    break;
                case 1:
                    KaMAjAk(KkAMAja + KKaMaja);
                    break;
                case 6:
                    n2 += kkamAJa[KkamAJa++];
                    if (KkamAJa == 3) {
                        KkamAJa = 0;
                    }
                    break;
                case 7:
                    kAmajAk(KKAMAja + kKaMaja);
                    break;
                case 8:
                    if (--KkaMAJa == 0) {
                        KAmajAk(false);
                        kkaMaja = 0;
                    }
                    break;
                case 9:
                    KaMAjAk(KkAMAja + KKaMaja);
                case 5: {
                    int n5 = module_channel_vibrato_table()[static_cast<std::size_t>(kKaMAJa & 0x1F)] *
                             kkaMAJa >> 5;
                    if ((kKaMAJa & 0x20) != 0) {
                        n5 = -n5;
                    }
                    n2 += n5;
                    kKaMAJa += KKaMAJa;
                    break;
                }
                case 11: {
                    int n6 = module_channel_vibrato_table()[static_cast<std::size_t>(KKAmaJa & 0x1F)] *
                             KkAmaJa >> 6;
                    if ((KKAmaJa & 0x20) != 0) {
                        n6 = -n6;
                    }
                    n = n6;
                    KKAmaJa += kkAmaJa;
                    break;
                }
            }
        }
        kaMajAk(n2);
        kamAjAk(n);
        kaMAjAk(n3);
    }
};

struct ModuleVoice : public ModuleChannel {
    ModuleInstrument* MAjAkKa;
    EnvelopeCursor mAjAkKa;
    EnvelopeCursor AmAjAkK;
    int amAjAkK;
    int AMAjAkK;
    int aMAjAkK;
    bool AmajAkK;

    ModuleVoice()
        : ModuleChannel(),
          MAjAkKa(NULL),
          mAjAkKa(),
          AmAjAkK(),
          amAjAkK(0),
          AMAjAkK(0),
          aMAjAkK(0),
          AmajAkK(false) {
    }

    void amajaKK(ModuleInstrument* mmjakmk2, int n) {
        MAjAkKa = mmjakmk2;
        if (MAjAkKa == NULL) {
            mAjAkKa.KKAMAja(NULL);
            AmAjAkK.KKAMAja(NULL);
            KAmAjAk();
            return;
        }
        mAjAkKa.KKAMAja(&MAjAkKa->akkamAJ);
        AmAjAkK.KKAMAja(&MAjAkKa->AKkamAJ);
        AMajaKK();
        int n2 = n < 0 ? 0 : (n > 95 ? 95 : n);
        ModuleSample* mmajmka2 = MAjAkKa->AkkamAJ[static_cast<std::size_t>(n2)];
        ModuleChannel::KAMAjAk(mmajmka2, n);
    }

    void kamajAk() {
        if (MAjAkKa == NULL) {
            return;
        }
        ModuleChannel::kamajAk();
    }

    void KAmAjAk() {
        if (MAjAkKa != NULL && mAjAkKa.JakkAMA) {
            mAjAkKa.KkAMAja();
            return;
        }
        ModuleChannel::KAmAjAk();
    }

    void AMajaKK() {
        if (MAjAkKa != NULL && MAjAkKa->AkKAmAJ > 0) {
            AmajAkK = true;
            amAjAkK = 0;
            AMAjAkK = MAjAkKa->akKAmAJ == 0 ? MAjAkKa->AkKAmAJ : 0;
            AmajaKK();
            return;
        }
        AmajAkK = false;
    }

    int AmajaKK() {
        if (MAjAkKa == NULL) {
            return 0;
        }
        if (MAjAkKa->akKAmAJ != 0) {
            AMAjAkK += MAjAkKa->AkKAmAJ / MAjAkKa->akKAmAJ;
            if (AMAjAkK > MAjAkKa->AkKAmAJ) {
                AMAjAkK = MAjAkKa->AkKAmAJ;
            }
        }
        int n = 0;
        switch (MAjAkKa->AKKAmAJ) {
            case 3:
                n = ((64 - (amAjAkK >> 1)) & 0x7F) - 64;
                break;
            case 2:
                n = ((64 + (amAjAkK >> 1)) & 0x7F) - 64;
                break;
            case 1:
                n = (amAjAkK & 0x80) != 0 ? 64 : -64;
                break;
            case 0:
                n = module_channel_wave_table()[static_cast<std::size_t>(amAjAkK & 0xFF)];
                break;
        }
        amAjAkK += MAjAkKa->aKkamAJ >> 2;
        return n * AMAjAkK >> 10;
    }

    void aMajaKK() {
        if (mAjAkKa.JakkAMA) {
            const int by = mAjAkKa.kKAMAja();
            KKAmAjA(by * kkAMAja >> 6);
        }
        if (AmAjAkK.JakkAMA) {
            const int by = AmAjAkK.kKAMAja();
            int n = KKAMAja < 128 ? KKAMAja : 255 - KKAMAja;
            n = (by - 32) * n >> 5;
            kkAmAjA(kKAMAja + n);
        }
        if (AmajAkK) {
            aMAjAkK = AmajaKK();
            KAMajAk(aMAjAkK);
        }
    }
};

struct ModulePattern {
    int KamAJAk;
    int kamAJAk;
    std::vector<std::uint8_t> KAmAJAk;

    ModulePattern()
        : KamAJAk(0),
          kamAJAk(0),
          KAmAJAk() {
    }

    ModulePattern(int n, int n2)
        : KamAJAk(n),
          kamAJAk(n2),
          KAmAJAk(static_cast<std::size_t>(n * n2 * 5), 0) {
    }

    void KAMAJak(int row, int channel, int note, int instrument, int effect, int param, int volume) {
        const std::size_t offset =
            (static_cast<std::size_t>(row) * static_cast<std::size_t>(kamAJAk) +
             static_cast<std::size_t>(channel)) *
            5U;
        KAmAJAk[offset] = static_cast<std::uint8_t>(note);
        KAmAJAk[offset + 1U] = static_cast<std::uint8_t>(instrument);
        KAmAJAk[offset + 2U] = static_cast<std::uint8_t>(effect);
        KAmAJAk[offset + 3U] = static_cast<std::uint8_t>(param);
        KAmAJAk[offset + 4U] = static_cast<std::uint8_t>(volume);
    }

    const std::uint8_t* kaMAJak(int row) const {
        return &KAmAJAk[(static_cast<std::size_t>(row) * static_cast<std::size_t>(kamAJAk)) * 5U];
    }
};

struct ModuleSong;

struct ModuleSequencer {
    std::vector<int> jAkkama;
    int JaKKama;
    ModuleSong* jaKKama;
    bool JAKKama;
    int jAKKama;
    int JakKama;

    ModuleSequencer()
        : jAkkama(),
          JaKKama(0),
          jaKKama(NULL),
          JAKKama(false),
          jAKKama(0),
          JakKama(0) {
    }

    void KkAMaJa(int n) {
        JaKKama = n;
    }

    void kKamaJa(ModuleSong* maajmmk2) {
        jaKKama = maajmmk2;
    }

    void KkaMaJa(const std::vector<std::uint8_t>& byArray, int n, int n2) {
        jAkkama.assign(static_cast<std::size_t>(n2), 0);
        for (int index = 0; index < n2; ++index) {
            jAkkama[static_cast<std::size_t>(index)] = byArray[static_cast<std::size_t>(n + index)] & 0xFF;
        }
    }

    void KKAMaJa() {
        kkamaJa(0);
    }

    void kkamaJa(int n) {
        if (jAkkama.empty()) {
            jAKKama = 0;
            JakKama = 0;
            return;
        }
        jAKKama = n < 0 ? 0 : (n < static_cast<int>(jAkkama.size()) ? n
                                                                    : static_cast<int>(jAkkama.size()) - 1);
        JakKama = 0;
    }

    void kKAMaJa(int n);
    void kkaMaJa();
    const std::uint8_t* kkAMaJa();
};

struct ModuleSong {
    ModuleSequencer jakKama;
    std::vector<ModulePattern> JAkKama;
    std::vector<ModuleSample> jAkKama;
    std::vector<ModuleInstrument> JaKkAMa;
    int jaKkAMa;
    int JAKkAMa;
    int jAKkAMa;
    std::string JakkAMa;
    int JAkkAMa;
    int jAkkAMa;
    int JaKKAMa;
    std::vector<ModuleVoice> jaKKAMa;
    bool JakKAMa;

    ModuleSong()
        : jakKama(),
          JAkKama(),
          jAkKama(),
          JaKkAMa(),
          jaKkAMa(0),
          JAKkAMa(125),
          jAKkAMa(6),
          JakkAMa(),
          JAkkAMa(6),
          jAkkAMa(125),
          JaKKAMa(0),
          jaKKAMa(),
          JakKAMa(false) {
    }

    void akKaMaJ() {
        JAkkAMa = jAKkAMa;
        jAkkAMa = JAKkAMa;
        jaKKAMa.assign(static_cast<std::size_t>(jaKkAMa), ModuleVoice());
        jakKama.KKAMaJa();
        jakKama.kKamaJa(this);
        JaKKAMa = 0;
    }

    static double aKKaMaJ(int n) {
        return 1.0 / (static_cast<double>(n) / 125.0) / 50.0;
    }

    double tick_duration_seconds() const {
        return aKKaMaJ(jAkkAMa);
    }

    ModuleInstrument* akkaMaJ(int n) {
        if (n < 0 || n >= static_cast<int>(JaKkAMa.size())) {
            return NULL;
        }
        return &JaKkAMa[static_cast<std::size_t>(n)];
    }

    void process_tick(bool* emitted_song_position, unsigned int* song_position_hex) {
        if (emitted_song_position != NULL) {
            *emitted_song_position = false;
        }
        if (song_position_hex != NULL) {
            *song_position_hex = 0U;
        }
        if (jaKKAMa.empty()) {
            return;
        }

        if (JaKKAMa-- == 0) {
            for (std::size_t index = 0; index < jaKKAMa.size(); ++index) {
                jaKKAMa[index].kAmAJAk(true);
            }
            const unsigned int current_song_position =
                (static_cast<unsigned int>(jakKama.jAKKama) << 8) |
                static_cast<unsigned int>(jakKama.JakKama);
            if (emitted_song_position != NULL) {
                *emitted_song_position = true;
            }
            if (song_position_hex != NULL) {
                *song_position_hex = current_song_position;
            }
            const std::uint8_t* by_array = jakKama.kkAMaJa();
            AkkaMaJ(by_array);
            JaKKAMa = JAkkAMa - 1;
        } else {
            for (std::size_t index = 0; index < jaKKAMa.size(); ++index) {
                jaKKAMa[index].kAmAJAk(false);
            }
        }
        for (std::size_t index = 0; index < jaKKAMa.size(); ++index) {
            jaKKAMa[index].aMajaKK();
        }
    }

    void mix_segment(const OfflineMad& mad,
                     std::vector<std::uint32_t>* buffer,
                     int start,
                     int end) {
        for (std::size_t index = 0; index < jaKKAMa.size(); ++index) {
            jaKKAMa[index].mix(mad, buffer, start, end);
        }
    }

    void AkkaMaJ(const std::uint8_t* byArray) {
        bool bl = false;
        int n = 0;
        bool bl2 = false;
        int n2 = 0;
        int n3 = 0;
        for (int n4 = 0; n4 < jaKkAMa; ++n4) {
            ModuleVoice& mmjjkka2 = jaKKAMa[static_cast<std::size_t>(n4)];
            const int by = byArray[n3 + 2] & 0xFF;
            const int n6 = byArray[n3] & 0xFF;
            int n5 = 0;

            if (n6 != 0 && n6 <= 96) {
                if (by != 3) {
                    n5 = byArray[n3 + 1] & 0xFF;
                    if (n5 == 0) {
                        mmjjkka2.kAMajAk(n6, false);
                    } else {
                        ModuleInstrument* mmjakmk2 = akkaMaJ(n5 - 1);
                        mmjjkka2.amajaKK(mmjakmk2, n6);
                        mmjjkka2.kKaMAJa = 0;
                        mmjjkka2.KKAmaJa = 0;
                    }
                    if (by == 9) {
                        mmjjkka2.KamajAk((byArray[n3 + 3] & 0xFF) << 8);
                    }
                } else if ((byArray[n3 + 1] & 0xFF) != 0) {
                    mmjjkka2.kamajAk();
                }
            } else {
                n5 = byArray[n3 + 1] & 0xFF;
                if (n5 > 0) {
                    mmjjkka2.kamajAk();
                    mmjjkka2.kKaMAJa = 0;
                    mmjjkka2.KKAmaJa = 0;
                }
                if (n6 > 96) {
                    mmjjkka2.KAmAjAk();
                }
            }

            n5 = byArray[n3 + 4] & 0xFF;
            if (n5 != 0) {
                if (n5 <= 80 && n5 >= 16) {
                    mmjjkka2.KaMAjAk(n5 - 16);
                } else {
                    const int n7 = n5 & 0xF;
                    switch ((n5 & 0xF0) >> 4) {
                        case 6:
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja - n7 * 2);
                            break;
                        case 7:
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja + n7 * 2);
                            break;
                        case 8:
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja - n7);
                            break;
                        case 9:
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja + n7);
                            break;
                        case 12:
                            mmjjkka2.kAmajAk(n7 << 4);
                            break;
                        case 13:
                            if (n7 != 0) {
                                mmjjkka2.kkaMaja = 7;
                                mmjjkka2.kKaMaja = -n7;
                            }
                            break;
                        case 14:
                            if (n7 != 0) {
                                mmjjkka2.kkaMaja = 7;
                                mmjjkka2.kKaMaja = n7;
                            }
                            break;
                    }
                }
            }

            const int n8 = byArray[n3 + 3] & 0xFF;
            switch (by) {
                case 0:
                    if (n8 != 0) {
                        mmjjkka2.kkaMaja = 6;
                        mmjjkka2.kkamAJa[0] = 0;
                        mmjjkka2.kkamAJa[1] = -(n8 & 0xF) * 64;
                        mmjjkka2.kkamAJa[2] = -((n8 & 0xF0) >> 4) * 64;
                        mmjjkka2.KkamAJa = 0;
                    }
                    break;
                case 1:
                    mmjjkka2.kkaMaja = 3;
                    if (n8 != 0) {
                        mmjjkka2.kkAmAJa = -n8 * 4;
                    }
                    break;
                case 2:
                    mmjjkka2.kkaMaja = 4;
                    if (n8 != 0) {
                        mmjjkka2.KKAmAJa = n8 * 4;
                    }
                    break;
                case 3:
                    if (mmjjkka2.kKamAja != NULL) {
                        mmjjkka2.kkaMaja = 2;
                        if (n8 != 0) {
                            mmjjkka2.KkAmAJa = (byArray[n3 + 1] & 0xFF) != 0 ? n8 << 2 : n8 << 1;
                        }
                        if (n6 != 0) {
                            mmjjkka2.kKAmAJa = mmjjkka2.kKamAja->KamajAK(n6);
                        }
                        if ((mmjjkka2.kKAmAJa < mmjjkka2.KKaMAja) ^
                            (mmjjkka2.KkAmAJa < 0)) {
                            mmjjkka2.KkAmAJa = -mmjjkka2.KkAmAJa;
                        }
                    }
                    break;
                case 4:
                    mmjjkka2.kkaMaja = 5;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.kkaMAJa = n8 & 0xF;
                    }
                    if ((n8 & 0xF0) != 0) {
                        mmjjkka2.KKaMAJa = (n8 & 0xF0) >> 4;
                    }
                    break;
                case 5:
                    mmjjkka2.kkaMaja = 10;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KKaMaja = -(n8 & 0xF);
                    }
                    if ((n8 & 0xF0) != 0) {
                        mmjjkka2.KKaMaja = n8 >> 4;
                    }
                    break;
                case 6:
                    mmjjkka2.kkaMaja = 9;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KKaMaja = -(n8 & 0xF);
                    }
                    if ((n8 & 0xF0) != 0) {
                        mmjjkka2.KKaMaja = n8 >> 4;
                    }
                    break;
                case 7:
                    mmjjkka2.kkaMaja = 11;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KkAmaJa = n8 & 0xF;
                    }
                    if ((n8 & 0xF0) != 0) {
                        mmjjkka2.kkAmaJa = (n8 & 0xF0) >> 4;
                    }
                    break;
                case 8:
                    mmjjkka2.kAmajAk(n8);
                    break;
                case 10:
                    mmjjkka2.kkaMaja = 1;
                    if ((n8 & 0xF) != 0) {
                        mmjjkka2.KKaMaja = -(n8 & 0xF);
                    }
                    if ((n8 & 0xF0) != 0) {
                        mmjjkka2.KKaMaja = n8 >> 4;
                    }
                    break;
                case 11:
                    bl2 = true;
                    n2 = n8;
                    break;
                case 12:
                    mmjjkka2.KaMAjAk(n8);
                    break;
                case 13:
                    bl = true;
                    n = n8;
                    break;
                case 14: {
                    const int n9 = n8 & 0xF;
                    switch ((n8 & 0xF0) >> 4) {
                        case 1:
                            if (n9 != 0) {
                                mmjjkka2.KKamAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja - mmjjkka2.KKamAJa * 4);
                            break;
                        case 2:
                            if (n9 != 0) {
                                mmjjkka2.kKamAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja + mmjjkka2.kKamAJa * 4);
                            break;
                        case 9:
                            if (n9 != 0) {
                                mmjjkka2.kkaMaja = 8;
                                mmjjkka2.KkaMAJa = n9;
                            } else {
                                mmjjkka2.KAmajAk(false);
                            }
                            break;
                        case 10:
                            if (n9 != 0) {
                                mmjjkka2.KkAMAJa = n9;
                            }
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja + mmjjkka2.KkAMAJa);
                            break;
                        case 11:
                            if (n9 != 0) {
                                mmjjkka2.kkAMAJa = n9;
                            }
                            mmjjkka2.KaMAjAk(mmjjkka2.KkAMAja - mmjjkka2.kkAMAJa);
                            break;
                    }
                    break;
                }
                case 15:
                    if (n8 < 32) {
                        JAkkAMa = n8;
                    } else {
                        jAkkAMa = n8;
                    }
                    break;
                case 33: {
                    const int n9 = n8 & 0xF;
                    switch ((n8 & 0xF0) >> 4) {
                        case 1:
                            if (n9 != 0) {
                                mmjjkka2.KKAMAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja - mmjjkka2.KKAMAJa);
                            break;
                        case 2:
                            if (n9 != 0) {
                                mmjjkka2.kKAMAJa = n9;
                            }
                            mmjjkka2.KamAjAk(mmjjkka2.KKaMAja + mmjjkka2.kKAMAJa);
                            break;
                    }
                    break;
                }
                default:
                    break;
            }

            n3 += 5;
        }

        if (bl) {
            jakKama.kkaMaJa();
            jakKama.kKAMaJa(n);
            return;
        }
        if (bl2) {
            jakKama.kkamaJa(n2);
        }
    }
};

void ModuleSequencer::kKAMaJa(int n) {
    if (jaKKama == NULL || jAkkama.empty()) {
        JakKama = 0;
        return;
    }
    const ModulePattern& majamka2 =
        jaKKama->JAkKama[static_cast<std::size_t>(jAkkama[static_cast<std::size_t>(jAKKama)])];
    JakKama = n < 0 ? 0 : (n < majamka2.KamAJAk ? n : majamka2.KamAJAk - 1);
}

void ModuleSequencer::kkaMaJa() {
    if (!JAKKama) {
        ++jAKKama;
    }
    if (jAKKama < static_cast<int>(jAkkama.size())) {
        kkamaJa(jAKKama);
        return;
    }
    if (JaKKama >= 0 && JaKKama < static_cast<int>(jAkkama.size())) {
        kkamaJa(JaKKama);
        return;
    }
    kkamaJa(0);
}

const std::uint8_t* ModuleSequencer::kkAMaJa() {
    ModulePattern& majamka2 =
        jaKKama->JAkKama[static_cast<std::size_t>(jAkkama[static_cast<std::size_t>(jAKKama)])];
    const std::uint8_t* by_array = majamka2.kaMAJak(JakKama);
    ++JakKama;
    if (JakKama == majamka2.KamAJAk) {
        kkaMaJa();
    }
    return by_array;
}

int kKaMaJA(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    return bytes[offset] & 0xFF;
}

int KkAmAja(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    return (bytes[offset] & 0xFF) | ((bytes[offset + 1U] & 0xFF) << 8);
}

int kKamAja(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    return (bytes[offset] & 0xFF) |
           ((bytes[offset + 1U] & 0xFF) << 8) |
           ((bytes[offset + 2U] & 0xFF) << 16) |
           ((bytes[offset + 3U] & 0xFF) << 24);
}

int KKAmAja(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    const std::uint16_t value =
        static_cast<std::uint16_t>((bytes[offset] & 0xFF) | ((bytes[offset + 1U] & 0xFF) << 8));
    return static_cast<std::int16_t>(value);
}

std::string decode_ascii(const std::vector<std::uint8_t>& bytes, std::size_t offset, std::size_t length) {
    std::string text;
    text.reserve(length);
    for (std::size_t index = 0; index < length && offset + index < bytes.size(); ++index) {
        const char ch = static_cast<char>(bytes[offset + index]);
        if (ch == '\0') {
            break;
        }
        text.push_back(ch);
    }
    return text;
}

bool read_file_bytes(const std::string& path,
                     std::vector<std::uint8_t>* bytes,
                     std::string* error_message) {
    std::ifstream stream(path.c_str(), std::ios::binary);
    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to read module asset: " + path;
        }
        return false;
    }

    stream.seekg(0, std::ios::end);
    const std::streamoff size = stream.tellg();
    stream.seekg(0, std::ios::beg);
    if (size < 0) {
        if (error_message != NULL) {
            *error_message = "unable to determine module asset size: " + path;
        }
        return false;
    }

    bytes->assign(static_cast<std::size_t>(size), 0);
    if (size == 0) {
        return true;
    }

    stream.read(reinterpret_cast<char*>(&(*bytes)[0]), size);
    if (!stream) {
        if (error_message != NULL) {
            *error_message = "unable to load module asset: " + path;
        }
        return false;
    }
    return true;
}

bool load_xm_song_from_bytes(const std::vector<std::uint8_t>& byArray,
                             ModuleSong* maajmmk2,
                             std::string* error_message) {
    if (byArray.size() < 80U) {
        if (error_message != NULL) {
            *error_message = "XM asset is truncated";
        }
        return false;
    }

    const std::string signature = decode_ascii(byArray, 0U, 17U);
    const int version = KkAmAja(byArray, 58U);
    if (signature != "Extended Module: " || version < 260) {
        if (error_message != NULL) {
            *error_message = "invalid XM module";
        }
        return false;
    }

    const std::string module_name = decode_ascii(byArray, 17U, 20U);
    const int header_size = kKamAja(byArray, 60U);
    const int song_length = KkAmAja(byArray, 64U);
    const int restart = KkAmAja(byArray, 66U);
    const int num_channels = KkAmAja(byArray, 68U);
    int num_patterns = KkAmAja(byArray, 70U);
    const int num_instruments = KkAmAja(byArray, 72U);
    const int initial_speed = KkAmAja(byArray, 76U);
    const int initial_tempo = KkAmAja(byArray, 78U);

    maajmmk2->jAKkAMa = initial_speed;
    maajmmk2->JAKkAMa = initial_tempo;
    maajmmk2->JakkAMa = module_name;
    maajmmk2->jaKkAMa = num_channels;

    ModuleSequencer kmajmmk2;
    kmajmmk2.KkAMaJa(restart);

    int n22 = 0;
    for (int index = 0; index < song_length; ++index) {
        const int pattern_index = byArray[80U + static_cast<std::size_t>(index)] & 0xFF;
        if (pattern_index > n22) {
            n22 = pattern_index;
        }
    }
    if (n22 >= num_patterns) {
        num_patterns = n22 + 1;
    }

    kmajmmk2.KkaMaJa(byArray, 80, song_length);
    maajmmk2->JAkKama.assign(static_cast<std::size_t>(num_patterns), ModulePattern());
    const ModulePattern empty_pattern(64, num_channels);
    std::size_t n26 = static_cast<std::size_t>(header_size + 60);

    for (int pattern_index = 0; pattern_index < num_patterns; ++pattern_index) {
        if (pattern_index < KkAmAja(byArray, 70U)) {
            const int pattern_header_size = kKamAja(byArray, n26);
            const int row_count = KkAmAja(byArray, n26 + 5U);
            const int packed_size = KkAmAja(byArray, n26 + 7U);
            maajmmk2->JAkKama[static_cast<std::size_t>(pattern_index)] =
                ModulePattern(row_count, num_channels);
            std::size_t n9 = n26 + static_cast<std::size_t>(pattern_header_size);
            for (int row = 0; row < row_count; ++row) {
                for (int channel = 0; channel < num_channels; ++channel) {
                    int note = 0;
                    int instrument = 0;
                    int effect = 0;
                    int param = 0;
                    int volume = 0;

                    const int control = byArray[n9++] & 0xFF;
                    int mask = control;
                    if ((control & 0x80) != 0) {
                        if ((mask & 1) != 0) {
                            note = byArray[n9++] & 0xFF;
                        }
                    } else {
                        note = control;
                        mask = 127;
                    }
                    if ((mask & 2) != 0) {
                        instrument = byArray[n9++] & 0xFF;
                    }
                    if ((mask & 4) != 0) {
                        volume = byArray[n9++] & 0xFF;
                    }
                    if ((mask & 8) != 0) {
                        effect = byArray[n9++] & 0xFF;
                    }
                    if ((mask & 0x10) != 0) {
                        param = byArray[n9++] & 0xFF;
                    }

                    maajmmk2->JAkKama[static_cast<std::size_t>(pattern_index)]
                        .KAMAJak(row, channel, note, instrument, effect, param, volume);
                }
            }
            n26 += static_cast<std::size_t>(pattern_header_size + packed_size);
        } else {
            maajmmk2->JAkKama[static_cast<std::size_t>(pattern_index)] = empty_pattern;
        }
    }

    maajmmk2->JaKkAMa.assign(static_cast<std::size_t>(num_instruments), ModuleInstrument());
    maajmmk2->jAkKama.assign(static_cast<std::size_t>(num_instruments * 20), ModuleSample());

    int sample_base_index = 0;
    for (int instrument_index = 0; instrument_index < num_instruments; ++instrument_index) {
        ModuleInstrument& instrument =
            maajmmk2->JaKkAMa[static_cast<std::size_t>(instrument_index)];
        const int instrument_header_size = kKamAja(byArray, n26);
        const std::string instrument_name = decode_ascii(byArray, n26 + 4U, 22U);
        const int sample_count = KkAmAja(byArray, n26 + 27U);
        n26 += 29U;
        int remainder = instrument_header_size - 29;
        int sample_header_size = 0;

        if (sample_count != 0) {
            int n42 = 0;
            bool bl = false;
            int n41 = 0;
            const std::uint8_t by3 = byArray[n26 + 204U];
            if ((by3 & 1U) != 0U) {
                n42 = 1;
            }
            if ((by3 & 4U) != 0U) {
                bl = true;
            }
            if ((by3 & 2U) != 0U) {
                n41 = 1;
            }
            const int n40 = byArray[n26 + 196U] & 0xFF;
            const int n39 = byArray[n26 + 198U] & 0xFF;
            const int by2 = byArray[n26 + 199U] & 0xFF;
            const int by = byArray[n26 + 200U] & 0xFF;
            std::vector<EnvelopePoint> point_array(static_cast<std::size_t>(n40));
            for (int n38 = 0; n38 < n40; ++n38) {
                const int x = KkAmAja(byArray, n26 + 100U + static_cast<std::size_t>(n38 * 4));
                const int y = KkAmAja(byArray, n26 + 102U + static_cast<std::size_t>(n38 * 4));
                point_array[static_cast<std::size_t>(n38)].x = x;
                point_array[static_cast<std::size_t>(n38)].y = y;
            }
            instrument.akkamAJ = Envelope(point_array, n39, by2, by);
            instrument.akkamAJ.majAKKa = n42 != 0;
            instrument.akkamAJ.MAjAKKa = bl;
            instrument.akkamAJ.mAjAKKa = n41 != 0;

            n42 = 0;
            bl = false;
            n41 = 0;
            const std::uint8_t by4 = byArray[n26 + 205U];
            if ((by4 & 1U) != 0U) {
                n42 = 1;
            }
            if ((by4 & 4U) != 0U) {
                bl = true;
            }
            if ((by4 & 2U) != 0U) {
                n41 = 1;
            }
            const int n37 = byArray[n26 + 197U] & 0xFF;
            const int n36 = byArray[n26 + 201U] & 0xFF;
            const int n35 = byArray[n26 + 202U] & 0xFF;
            const int n34 = byArray[n26 + 203U] & 0xFF;
            point_array.assign(static_cast<std::size_t>(n37), EnvelopePoint());
            for (int n33 = 0; n33 < n37; ++n33) {
                const int x = KkAmAja(byArray, n26 + 148U + static_cast<std::size_t>(n33 * 4));
                const int y = KkAmAja(byArray, n26 + 150U + static_cast<std::size_t>(n33 * 4));
                point_array[static_cast<std::size_t>(n33)].x = x;
                point_array[static_cast<std::size_t>(n33)].y = y;
            }
            instrument.AKkamAJ = Envelope(point_array, n36, n35, n34);
            instrument.AKkamAJ.majAKKa = n42 != 0;
            instrument.AKkamAJ.MAjAKKa = bl;
            instrument.AKkamAJ.mAjAKKa = n41 != 0;

            instrument.akkamAJ.amAjakK(KkAmAja(byArray, n26 + 210U));
            instrument.AKKAmAJ = kKaMaJA(byArray, n26 + 206U);
            instrument.akKAmAJ = kKaMaJA(byArray, n26 + 207U);
            instrument.AkKAmAJ = (kKaMaJA(byArray, n26 + 208U) & 0xF) << 4;
            instrument.aKkamAJ = (kKaMaJA(byArray, n26 + 209U) & 0x3F) << 2;

            sample_header_size = kKamAja(byArray, n26);
            for (int note = 0; note < 96; ++note) {
                const int sample_index = kKaMaJA(byArray, n26 + 4U + static_cast<std::size_t>(note));
                instrument.AkkamAJ[static_cast<std::size_t>(note)] =
                    &maajmmk2->jAkKama[static_cast<std::size_t>(sample_base_index + sample_index)];
            }

            n26 += 214U;
            remainder -= 214;
        } else {
            instrument.routineRegistry(NULL);
        }

        n26 += static_cast<std::size_t>(remainder);
        const std::size_t sample_data_offset = n26 + static_cast<std::size_t>(sample_count * sample_header_size);
        std::size_t sample_data_cursor = sample_data_offset;

        for (int sample_index = 0; sample_index < sample_count; ++sample_index) {
            ModuleSample& sample =
                maajmmk2->jAkKama[static_cast<std::size_t>(sample_base_index + sample_index)];
            const int sample_length_bytes = kKamAja(byArray, n26);
            int loop_start = kKamAja(byArray, n26 + 4U);
            int loop_length = kKamAja(byArray, n26 + 8U);
            const int volume = static_cast<std::int8_t>(byArray[n26 + 12U]);
            const int fine_tune = static_cast<std::int8_t>(byArray[n26 + 13U]);
            const std::uint8_t flags = byArray[n26 + 14U];
            const int panning = byArray[n26 + 15U] & 0xFF;
            const int relative_note = static_cast<std::int8_t>(byArray[n26 + 16U]);
            const std::string sample_name = decode_ascii(byArray, n26 + 18U, 22U);

            sample.kAmAJAK(sample_name.empty() ? instrument_name : sample_name);
            sample.KAmAJAK(relative_note);
            sample.KaMajAK(volume);
            sample.KAMajAK(fine_tune);
            sample.KAmajAK(panning);

            std::vector<std::int8_t> decoded_sample;
            if ((flags & 0x10U) != 0U) {
                const int sample_count_frames = sample_length_bytes / 2;
                loop_start /= 2;
                loop_length /= 2;
                decoded_sample.assign(static_cast<std::size_t>(sample_count_frames), 0);
                int accumulator = 0;
                std::size_t source = sample_data_cursor;
                for (int i = 0; i < sample_count_frames; ++i) {
                    accumulator += KKAmAja(byArray, source);
                    decoded_sample[static_cast<std::size_t>(i)] =
                        static_cast<std::int8_t>(accumulator >> 8);
                    source += 2U;
                }
            } else {
                decoded_sample.assign(static_cast<std::size_t>(sample_length_bytes), 0);
                int accumulator = 0;
                std::size_t source = sample_data_cursor;
                for (int i = 0; i < sample_length_bytes; ++i) {
                    accumulator += static_cast<std::int8_t>(byArray[source++]);
                    decoded_sample[static_cast<std::size_t>(i)] =
                        static_cast<std::int8_t>(accumulator);
                }
            }

            sample.kAMajAK(decoded_sample);
            sample.kaMajAK(flags & 0x3U, loop_start, loop_length);

            sample_data_cursor += static_cast<std::size_t>(sample_length_bytes);
            n26 += static_cast<std::size_t>(sample_header_size);
        }

        n26 = sample_data_cursor;
        sample_base_index += sample_count;
    }

    maajmmk2->jakKama = kmajmmk2;
    maajmmk2->jakKama.kKamaJa(maajmmk2);
    return true;
}

struct SequenceModuleConfig {
    const char* module_path;
    int boost;
    bool has_start_song_position;
    unsigned int start_song_position_hex;
};

bool lookup_sequence_module_config(const std::string& sequence_name,
                                   SequenceModuleConfig* config) {
    if (sequence_name == "intro") {
        config->module_path = "original/forward/mods/kuninga.xm";
        config->boost = 88;
        config->has_start_song_position = false;
        config->start_song_position_hex = 0U;
        return true;
    }
    if (sequence_name == "saari") {
        config->module_path = "original/forward/mods/jarnomix.xm";
        config->boost = 128;
        config->has_start_song_position = false;
        config->start_song_position_hex = 0U;
        return true;
    }
    if (sequence_name == "kukot") {
        config->module_path = "original/forward/mods/jarnomix.xm";
        config->boost = 128;
        config->has_start_song_position = true;
        config->start_song_position_hex = 0x0700U;
        return true;
    }
    if (sequence_name == "maku") {
        config->module_path = "original/forward/mods/jarnomix.xm";
        config->boost = 128;
        config->has_start_song_position = true;
        config->start_song_position_hex = 0x0D00U;
        return true;
    }
    if (sequence_name == "watercube") {
        config->module_path = "original/forward/mods/jarnomix.xm";
        config->boost = 128;
        config->has_start_song_position = true;
        config->start_song_position_hex = 0x1000U;
        return true;
    }
    if (sequence_name == "feta") {
        config->module_path = "original/forward/mods/jarnomix.xm";
        config->boost = 128;
        config->has_start_song_position = true;
        config->start_song_position_hex = 0x1230U;
        return true;
    }
    return false;
}

bool load_sequence_module_song(const std::string& sequence_name,
                               ModuleSong* song,
                               int* boost,
                               SequenceModuleConfig* config_out,
                               std::string* error_message) {
    SequenceModuleConfig config;
    if (!lookup_sequence_module_config(sequence_name, &config)) {
        if (error_message != NULL) {
            *error_message = "no native module mapping for sequence: " + sequence_name;
        }
        return false;
    }

    std::vector<std::uint8_t> module_bytes;
    if (!read_file_bytes(config.module_path, &module_bytes, error_message)) {
        return false;
    }
    if (!load_xm_song_from_bytes(module_bytes, song, error_message)) {
        return false;
    }

    if (boost != NULL) {
        *boost = config.boost;
    }
    if (config_out != NULL) {
        *config_out = config;
    }
    return true;
}

bool render_module_song(const ModuleSong& song_template,
                        int sample_rate,
                        int boost,
                        std::size_t sample_frames,
                        std::vector<std::int16_t>* interleaved_samples,
                        std::vector<SongPositionEvent>* song_positions) {
    ModuleSong song(song_template);
    song.akKaMaJ();

    OfflineMad mad;
    mad.frequency = sample_rate;
    mad.stereo = true;
    mad.boost = boost;

    std::vector<std::uint32_t> packed_frames(sample_frames, 0U);
    double next_tick_sample = 0.0;
    std::size_t cursor = 0U;
    if (song_positions != NULL) {
        song_positions->clear();
    }

    while (cursor < sample_frames) {
        const std::size_t boundary = static_cast<std::size_t>(next_tick_sample);
        if (boundary > cursor) {
            const std::size_t segment_end = boundary < sample_frames ? boundary : sample_frames;
            song.mix_segment(mad,
                             &packed_frames,
                             static_cast<int>(cursor),
                             static_cast<int>(segment_end));
            cursor = segment_end;
            if (cursor >= sample_frames) {
                break;
            }
        }

        bool emitted_song_position = false;
        unsigned int song_position_hex = 0U;
        song.process_tick(&emitted_song_position, &song_position_hex);
        if (emitted_song_position && song_positions != NULL) {
            SongPositionEvent event;
            event.sample_index = static_cast<std::uint64_t>(cursor);
            event.song_position_hex = song_position_hex;
            song_positions->push_back(event);
        }
        next_tick_sample += song.tick_duration_seconds() * static_cast<double>(sample_rate);
        if (boundary <= cursor && static_cast<std::size_t>(next_tick_sample) == cursor) {
            next_tick_sample += song.tick_duration_seconds() * static_cast<double>(sample_rate);
        }
    }

    interleaved_samples->assign(sample_frames * 2U, 0);
    for (std::size_t index = 0; index < sample_frames; ++index) {
        const std::uint32_t frame = packed_frames[index];
        (*interleaved_samples)[index * 2U] =
            static_cast<std::int16_t>(static_cast<std::uint16_t>(frame & 0xFFFFU));
        (*interleaved_samples)[index * 2U + 1U] =
            static_cast<std::int16_t>(static_cast<std::uint16_t>((frame >> 16) & 0xFFFFU));
    }

    return true;
}

bool find_song_position_sample_index(const ModuleSong& song_template,
                                     int sample_rate,
                                     unsigned int target_song_position_hex,
                                     std::uint64_t* sample_index,
                                     std::string* error_message) {
    if (sample_index == NULL) {
        if (error_message != NULL) {
            *error_message = "song-position sample-index output is null";
        }
        return false;
    }
    if (sample_rate <= 0) {
        if (error_message != NULL) {
            *error_message = "invalid module playback sample rate";
        }
        return false;
    }

    ModuleSong song(song_template);
    song.akKaMaJ();

    double next_tick_sample = 0.0;
    const std::size_t kMaxSongRows = 200000U;
    std::size_t emitted_rows = 0U;

    while (emitted_rows < kMaxSongRows) {
        const std::size_t boundary = static_cast<std::size_t>(next_tick_sample);
        bool emitted_song_position = false;
        unsigned int song_position_hex = 0U;
        song.process_tick(&emitted_song_position, &song_position_hex);
        if (emitted_song_position) {
            if (song_position_hex == target_song_position_hex) {
                *sample_index = static_cast<std::uint64_t>(boundary);
                return true;
            }
            ++emitted_rows;
        }

        next_tick_sample += song.tick_duration_seconds() * static_cast<double>(sample_rate);
        if (static_cast<std::size_t>(next_tick_sample) == boundary) {
            next_tick_sample += song.tick_duration_seconds() * static_cast<double>(sample_rate);
        }
    }

    if (error_message != NULL) {
        *error_message = "unable to resolve song position from native module timeline";
    }
    return false;
}

}  // namespace

bool resolve_sequence_frame_count_for_song_position(const std::string& sequence_name,
                                                    int fps,
                                                    int sample_rate,
                                                    unsigned int song_position_hex,
                                                    int post_roll_frames,
                                                    int* frame_count,
                                                    std::string* error_message) {
    if (frame_count == NULL) {
        if (error_message != NULL) {
            *error_message = "frame-count output is null";
        }
        return false;
    }
    if (fps <= 0) {
        if (error_message != NULL) {
            *error_message = "fps must be positive";
        }
        return false;
    }
    if (sample_rate <= 0 || (sample_rate % fps) != 0) {
        if (error_message != NULL) {
            *error_message = "sample rate must be divisible by fps";
        }
        return false;
    }
    if (post_roll_frames < 0) {
        if (error_message != NULL) {
            *error_message = "post-roll frames must be non-negative";
        }
        return false;
    }

    ModuleSong song;
    SequenceModuleConfig config;
    if (!load_sequence_module_song(sequence_name, &song, NULL, &config, error_message)) {
        return false;
    }

    std::uint64_t start_sample_index = 0ULL;
    if (config.has_start_song_position &&
        !find_song_position_sample_index(song,
                                         sample_rate,
                                         config.start_song_position_hex,
                                         &start_sample_index,
                                         error_message)) {
        return false;
    }

    std::uint64_t end_sample_index = 0ULL;
    if (!find_song_position_sample_index(song,
                                         sample_rate,
                                         song_position_hex,
                                         &end_sample_index,
                                         error_message)) {
        return false;
    }
    if (end_sample_index < start_sample_index) {
        if (error_message != NULL) {
            *error_message = "target song position resolves before the sequence start";
        }
        return false;
    }

    const int samples_per_frame = sample_rate / fps;
    const std::uint64_t pre_roll_frame_count =
        ((end_sample_index - start_sample_index) + static_cast<std::uint64_t>(samples_per_frame) - 1ULL) /
        static_cast<std::uint64_t>(samples_per_frame);
    const std::uint64_t total_frame_count =
        pre_roll_frame_count + static_cast<std::uint64_t>(post_roll_frames);

    *frame_count = static_cast<int>(total_frame_count);
    return true;
}

bool render_sequence_module_audio(const std::string& sequence_name,
                                  int sample_rate,
                                  std::size_t sample_frames,
                                  SequenceAudioRender* render,
                                  std::string* error_message) {
    if (render == NULL) {
        if (error_message != NULL) {
            *error_message = "audio output container is null";
        }
        return false;
    }

    render->interleaved_samples.clear();
    render->song_positions.clear();
    if (sample_frames == 0U) {
        return true;
    }
    if (sample_rate <= 0) {
        if (error_message != NULL) {
            *error_message = "invalid module playback sample rate";
        }
        return false;
    }

    ModuleSong song;
    int boost = 0;
    SequenceModuleConfig config;
    if (!load_sequence_module_song(sequence_name, &song, &boost, &config, error_message)) {
        return false;
    }

    std::uint64_t start_sample_index = 0ULL;
    if (config.has_start_song_position &&
        !find_song_position_sample_index(song,
                                         sample_rate,
                                         config.start_song_position_hex,
                                         &start_sample_index,
                                         error_message)) {
        return false;
    }

    const std::size_t total_sample_frames =
        static_cast<std::size_t>(start_sample_index) + sample_frames;
    std::vector<std::int16_t> full_interleaved_samples;
    std::vector<SongPositionEvent> full_song_positions;
    if (!render_module_song(song,
                            sample_rate,
                            boost,
                            total_sample_frames,
                            &full_interleaved_samples,
                            &full_song_positions)) {
        if (error_message != NULL) {
            *error_message = "native module render failed";
        }
        return false;
    }

    if (start_sample_index == 0ULL) {
        render->interleaved_samples.swap(full_interleaved_samples);
        render->song_positions.swap(full_song_positions);
        return true;
    }

    const std::size_t start_pcm_index = static_cast<std::size_t>(start_sample_index) * 2U;
    render->interleaved_samples.assign(full_interleaved_samples.begin() + start_pcm_index,
                                       full_interleaved_samples.end());
    render->song_positions.clear();
    for (std::size_t index = 0; index < full_song_positions.size(); ++index) {
        if (full_song_positions[index].sample_index < start_sample_index) {
            continue;
        }

        SongPositionEvent event = full_song_positions[index];
        event.sample_index -= start_sample_index;
        render->song_positions.push_back(event);
    }

    return true;
}

}  // namespace forward_offline
