//
// Created by chris on 2025-12-08.
//

#ifndef TEST_APP_SAFETY_HPP
#define TEST_APP_SAFETY_HPP

//DO NOT USE THIS except to avoid an unused warning
//when transitioning to an emergency state where
//the result of the transition should be discarded
//in favor of making sure the transition occurs.
#define DISCARD(x) (void)(x)

#endif //TEST_APP_SAFETY_HPP
