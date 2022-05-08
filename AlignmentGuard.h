#pragma once

#include <cstdint>

/**
 * Enables alignment checks until the object goes out of scope.
 */
class AlignmentGuard {
   public:
    AlignmentGuard() {
#if defined(__i386__)
        uint32_t eflags = 0;
        __asm__(
            "pushf \n"
            "mov (%%esp), %0 \n"
            "orl $0x40000, (%%esp) \n"
            "popf \n"
            : "=r"(eflags)::);
#elif defined(__x86_64__)
        uint64_t eflags = 0;
        __asm__(
            "pushf \n"
            "mov (%%rsp), %0 \n"
            "orl $0x40000, (%%rsp) \n"
            "popf \n"
            : "=r"(eflags)::);
#else
#error "Not supported CPU architecture"
#endif
        enabled = !(eflags & 0x40000);
    }

    ~AlignmentGuard() {
        if (!enabled) {
            return;
        }

#if defined(__i386__)
        __asm__(
            "pushf \n"
            "andl $0xfffbffff, (%%esp) \n"
            "popf \n" ::
                :);
#elif defined(__x86_64__)
        __asm__(
            "pushf \n"
            "andl $0xfffbffff, (%%rsp) \n"
            "popf \n" ::
                :);
#endif
    }

   private:
    AlignmentGuard(AlignmentGuard&) = delete;
    AlignmentGuard(AlignmentGuard&&) = delete;
    AlignmentGuard& operator=(AlignmentGuard&) = delete;
    AlignmentGuard& operator=(AlignmentGuard&&) = delete;

    /**
     * True if #AC bit has been set by the constructor.
     */
    bool enabled{};
};
