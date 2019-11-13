//
// Created by patr0nus on 2019/11/13.
//

#ifndef DESKGAP_APP_H
#define DESKGAP_APP_H

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct {
        void (*on_ready)(const void*);
        void (*before_quit)(const void*);
    } dg_app_event_callbacks;

    void dg_app_run(dg_app_event_callbacks event_callbacks, const void* user_data);
#ifdef __cplusplus
}
#endif

#endif //DESKGAP_APP_H
