#ifndef DO_KEY_DEPRECATED_H_INCLUDED
#define DO_KEY_DEPRECATED_H_INCLUDED


typedef struct {
    char *name;
    char *width;
    char *height;
}do_key_model_t;

#define MAX_MODEL 4
const do_key_model_t key_models[MAX_MODEL] =
{ {"Fujitsu","11.5mm","11.5mm"},
  {"IBM","19.5mm","12.5mm"},
  {"Posiflex","13mm","13mm"},
  {"BTC","22mm","13mm"}
};
const char *default_font="Tahoma 12";

#endif // DO_KEY_DEPRECATED_H_INCLUDED
