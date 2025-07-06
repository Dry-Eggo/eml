#pragma once

#define shr(type) std::shared_ptr<type>
#define as(type, variant) (std::get<type>(variant->data))
#define uniq std::unique_ptr
#define mk_uniq(type, ...) std::make_unique<type>(__VA_ARGS__)
