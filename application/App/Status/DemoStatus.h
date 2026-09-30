#pragma once

#include "Pages/StatusBar/StatusBar.h"

#include <cstdint>

/** Deterministic placeholder status producer used until the real backend exists. */
class DemoStatus {
  public:
    /**
     * Build a synthetic status snapshot from monotonic elapsed milliseconds.
     * The result contains no hardware or business data and is safe to copy.
     */
    static page::StatusBarState sample(std::uint64_t elapsed_ms);
};
