#pragma once

#ifndef CF_PLAIN_PROPERTY
#    define CF_PLAIN_PROPERTY(val_type, val_name, default_value) \
      public:                                                    \
        val_type& get_##val_name() {                             \
            return val_name;                                     \
        }                                                        \
        const val_type& get_##val_name##_const() const {         \
            return val_name;                                     \
        }                                                        \
        void set_##val_name(const val_type& v) {                 \
            val_name = v;                                        \
        }                                                        \
                                                                 \
      private:                                                   \
        val_type val_name{default_value};

#endif