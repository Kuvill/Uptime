#pragma once

void blockInteruptSignals();
void allowInteruptSignals();

void setCallbackForInteruptSignals( void (*)( int ) );
