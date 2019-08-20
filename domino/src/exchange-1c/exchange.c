#include "exchange.h"
#include "do_message.h"
#include "misc.h"
#include "exchange-const.h"


struct {
  ExchangeType first;
  ExchangeType last;
} exchange_type_kind[EXCHANGE_TYPE_KIND_NO] =
{
    {EXCHANGE_TYPE_HANDBOOK_CONTRACT, EXCHANGE_TYPE_HANDBOOK_COUNTRY},
    {EXCHANGE_TYPE_LIST_UNIT_KIND, EXCHANGE_TYPE_LIST_PERSON_KIND},
    {EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT},
    {EXCHANGE_TYPE_ACCOUNT_DEFAULT, EXCHANGE_TYPE_ACCOUNT_DEFAULT},
    {EXCHANGE_TYPE_CONST_BOOLEAN, EXCHANGE_TYPE_CONST_STRING},
};

static gchar *exchange_type_name[EXCHANGE_TYPE_NO] =
{
    "СправочникСсылка.ДоговорыКонтрагентов",
    "СправочникСсылка.Контрагенты",
    "СправочникСсылка.БанковскиеСчета",
    "СправочникСсылка.Организации",
    "СправочникСсылка.ТипыЦенНоменклатуры",
    "СправочникСсылка.Склады",
    "СправочникСсылка.НоменклатурныеГруппы",
    "СправочникСсылка.КлассификаторЕдиницИзмерения",
    "СправочникСсылка.Номенклатура",
    "СправочникСсылка.СтатьиЗатрат",
    "СправочникСсылка.КонтактныеЛица",
    "СправочникСсылка.НомераГТД",
    "СправочникСсылка.СпецификацииНоменклатуры",
    "СправочникСсылка.ФизическиеЛица",
    "СправочникСсылка.ПодразделенияОрганизаций",
    "СправочникСсылка.ДолжностиОрганизаций",
    "СправочникСсылка.СотрудникиОрганизаций",
    "СправочникСсылка.Валюты",
    "СправочникСсылка.Пользователи",
    "СправочникСсылка.ПрочиеДоходыИРасходы",
    "СправочникСсылка.СтатьиДвиженияДенежныхСредств",
    "СправочникСсылка.ВидыОплатОрганизаций",
    "СправочникСсылка.КлассификаторСтранМира",

    "ПеречислениеСсылка.ВидыСкладов",
    "ПеречислениеСсылка.СтавкиНДС",
    "ПеречислениеСсылка.ВидыЗанятостиВОрганизации",
    "ПеречислениеСсылка.ВидыОперацийПоступлениеТоваровУслуг",
    "ПеречислениеСсылка.ВидыОперацийПеремещениеТоваров",
    "ПеречислениеСсылка.СпособыЗачетаАвансов",
    "ПеречислениеСсылка.ВидыДоговоровКонтрагентов",
    "ПеречислениеСсылка.ОтражениеВУСН",
    "ПеречислениеСсылка.ДействиеНДСВСтоимостиТоваров",
    "ПеречислениеСсылка.ВидыДеятельностиДляНалоговогоУчетаЗатрат",
    "ПеречислениеСсылка.ВидыПрочихДоходовИРасходов",
    "ПеречислениеСсылка.ВидыОперацийСписаниеТоваров",
    "ПеречислениеСсылка.ВидыОперацийВозвратТоваровПоставщику",
    "ПеречислениеСсылка.ВидыДвиженийДенежныхСредств",
    "ПеречислениеСсылка.ТипыОплат",
    "ПеречислениеСсылка.ВидыОперацийОтчетОРозничныхПродажах",
    "ПеречислениеСсылка.ЮрФизЛицо",

    "ПланВидовХарактеристикСсылка.ВидыСубконтоХозрасчетные",

    "ПланСчетовСсылка.Хозрасчетный",

    "Булево",
    "Дата",
    "Число",
    "УникальныйИдентификатор",
    "Строка",


};
const gchar *exchange_type_get_name(ExchangeType type)
{
    return exchange_type_name[type];
}

/*
static ExchangeType property_name_required[] =
{
    EXCHANGE_TYPE_HANDBOOK_PARTNER,
    EXCHANGE_TYPE_HANDBOOK_FIRM,
    EXCHANGE_TYPE_HANDBOOK_PRICE_TYPE,
    EXCHANGE_TYPE_HANDBOOK_UNIT,
    EXCHANGE_TYPE_HANDBOOK_PRODUCT_GROUP,
    EXCHANGE_TYPE_HANDBOOK_EI,
    EXCHANGE_TYPE_HANDBOOK_PRODUCT,
    EXCHANGE_TYPE_HANDBOOK_COST,
    EXCHANGE_TYPE_HANDBOOK_DECLARATION,
    EXCHANGE_TYPE_HANDBOOK_PRODUCT_SPECIFICATION,
    EXCHANGE_TYPE_HANDBOOK_CONTACTS,
    EXCHANGE_TYPE_HANDBOOK_DECLARATION,
    EXCHANGE_TYPE_HANDBOOK_PERSON_NATURAL,
    EXCHANGE_TYPE_HANDBOOK_COUNTRY,
};
*/
static gchar *get_text(xmlNode *node)
{
    gchar *res = NULL;
    if ( node->type == XML_TEXT_NODE ) {
        if ( node->content != NULL )
            res=g_strdup((gchar*)node->content);
        else
            res=g_strdup("");
    }
    if ( !res ) {
        xmlNode *cur;
        GString *str = g_string_new("");
        cur = node->xmlChildrenNode;
        while ( cur ) {
            str=g_string_append(str, get_text(cur));
            cur = cur->next;
        }
        res = g_string_free(str, FALSE);
    }
    /*
    gchar *p;
    gunichar ch;
    p = res;
    for ( ;*p != '\0'; p = g_utf8_next_char (p) ) { // trim space
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            break;
    }
    p = g_strdup(p);
    g_free(res);
    res = p;
    p = res + strlen(res) - 1;
    for ( ;p > res; p = g_utf8_prev_char(p) ) { // trim space
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            break;
    }
    if ( p >= res ) {
        ch = g_utf8_get_char (p);
        if ( !g_unichar_isspace (ch) )
            p = g_utf8_next_char(p);
        *p = '\0';
    }
    */
    return res;

}
static gchar *get_attribute(xmlNode *node, const gchar *name)
{
    xmlAttrPtr prev = node->properties;
    while ( prev ) {
        if ( prev->type == XML_ATTRIBUTE_NODE ) {
            if ( str_iqual((char*)prev->name, name) ) {
                if ( prev->children )
                    return get_text(prev->children);
                return NULL;
            }
        }
        prev = prev->next;
    }
    return NULL;
}
ExchangeTypeKind exchange_type_get_kind(ExchangeType type)
{
    int j;
    for ( j = 0; j < EXCHANGE_TYPE_KIND_NO; j++ ) {
        if ( (exchange_type_kind[j].first <= type) && (type <= exchange_type_kind[j].last) ) {
            return j;
        }
    }
    return EXCHANGE_TYPE_KIND_NO;
}
static gboolean get_type(xmlNode *node, ExchangeType *type, ExchangeTypeKind *kind)
{
    gchar *type_name;

    type_name = get_attribute(node, ELEM_1C_TYPE);
    if ( !type_name ) {
        MY_ERROR("Строка %d. У элемента %s неопределен тип. ", node->line, node->name);
        return FALSE;
    }
    int i,j;
    for ( i = 0; i < EXCHANGE_TYPE_NO; i++ ) {
        if ( str_iqual(type_name, exchange_type_name[i]) ) {
            g_free(type_name);
            for ( j = 0; j < EXCHANGE_TYPE_KIND_NO; j++ ) {
                if ( (exchange_type_kind[j].first <= i) && (i <= exchange_type_kind[j].last) ) {
                    *kind = j;
                    *type = i;
                    return TRUE;
                }
            }
            MY_ERROR("Строка %d. Неизвестный вид \"%s\".", node->line, type_name);
            return FALSE;
        }
    }
    MY_ERROR("Строка %d. Неизвестный тип \"%s\".", node->line, type_name);
    g_free(type_name);
    return FALSE;
}
static ExchangeObject *exchange_object_new(ExchangeType type,  ExchangeTypeKind kind, gint line)
{
    ExchangeObject *res;
    res = g_new0(ExchangeObject, 1);
    res->type = type;
    res->kind = kind;
    res->line = line;
    return res;
}
static gboolean exchange_property_free(ExchangeProperty *property)
{
    g_free(property);
    return TRUE;
}
static gboolean exchange_object_free(ExchangeObject *obj)
{
    g_slist_foreach(obj->properties, (GFunc)exchange_property_free, NULL);
    g_free(obj);
    return TRUE;
}
void exchange_objects_free(GSList *objs)
{
    g_slist_foreach(objs, (GFunc)exchange_object_free, NULL);
    g_slist_free(objs);
}

gboolean parse_obj(xmlNode *node, ExchangeObject **obj)
{
    *obj = NULL;
    if ( CANCELLED )
        return FALSE;

    if ( node->type != XML_ELEMENT_NODE )
        return TRUE;

    if ( str_iqual((char*)node->name, ELEM_1C_RULES) )
        return TRUE;

    if ( !str_iqual((char*)node->name, ELEM_1C_OBJECT) ) {
        MY_ERROR("Строка %d. Неправильный элемент \"%s\".", node->line, node->name);
        return FALSE;
    }

    gchar *type_name;

    type_name = get_attribute(node, ELEM_1C_TYPE);
    if ( !type_name ) {
        MY_ERROR("Строка %d. У элемента %s неопределен тип. ", node->line, node->name);
        return FALSE;
    }
    ExchangeType type;
    ExchangeTypeKind kind;

    if ( !get_type(node, &type, &kind) )
        return FALSE;

    if ( kind != EXCHANGE_TYPE_KIND_HANDBOOK &&
         kind != EXCHANGE_TYPE_KIND_ACCOUNT &&
         kind != EXCHANGE_TYPE_KIND_ATTRIBUTE  ) {
        MY_ERROR("Строка %d. Ожидалось определение обьекта.", node->line);
        return FALSE;
    }
    *obj = exchange_object_new(type, kind, node->line);

    xmlNode *child;
    ExchangeProperty *property;

    child = node->children;

    while ( child ) {
        if ( !parse_property(child, kind, &property, &(*obj)->key) ) {
            exchange_object_free(*obj);
            return FALSE;
        }
        if ( property )
            (*obj)->properties = g_slist_append((*obj)->properties, property);
        child = child->next;
    }
    if ( !(*obj)->key ) {
        exchange_object_free(*obj);
        MY_ERROR("Строка %d. Неопределена ссылка на обьект.", node->line);
        return FALSE;
    }
    return TRUE;
}
gboolean parse_property(xmlNode *node, ExchangeTypeKind owner_kind, ExchangeProperty **property, ExchangeKey **key)
{
    *property = NULL;

    if ( node->type != XML_ELEMENT_NODE )
        return TRUE;

    if ( str_iqual((char*)node->name, ELEM_1C_LINK) && key ) {
        return parse_key(node, owner_kind, key);
    }

    if ( !str_iqual((char*)node->name, ELEM_1C_PROPERTY) ) {
        MY_ERROR("Строка %d. Неправильный элемент \"%s\". Ожидалось \"%s\"", node->line, node->name, ELEM_1C_PROPERTY);
        return FALSE;
    }
    (*property) = g_new0(ExchangeProperty, 1);
    (*property)->name = get_attribute(node, ELEM_1C_NAME_S);
    (*property)->line = node->line;
    if ( !(*property)->name || (*property)->name[0] == '\0' ) {
        MY_ERROR("Строка %d. Непределено имя элемента \"%s\".", node->line, node->name);
        exchange_property_free(*property);
        return FALSE;
    }

    if ( !get_type(node, &(*property)->type, &(*property)->kind) )
        return FALSE;

    if ( !parse_value(node->children, (*property)->kind, *property) ) {
        MY_ERROR("Строка %d. Непределено значение элемента \"%s\".", node->line, (*property)->name);
        exchange_property_free(*property);
        return FALSE;
    }
    return TRUE;

}
static gboolean exchange_key_free(ExchangeKey *key)
{
    g_free(key->guid);
    g_free(key->code);
    g_free(key);
    return TRUE;
}

gboolean parse_key(xmlNode *node, ExchangeTypeKind kind, ExchangeKey **key)
{

    if ( !node || node->type != XML_ELEMENT_NODE ) {
        MY_ERROR("Строка %d. Логическая ошибка в определении сылки", node->line);
        return FALSE;
    }
    if ( !str_iqual((char*)node->name, ELEM_1C_LINK) ) {
        MY_ERROR("Строка %d. Неправильный элемент \"%s\". Ожидалось \"%s\"", node->line, node->name, ELEM_1C_LINK);
        return FALSE;
    }
    *key = g_new0(ExchangeKey, 1);
    (*key)->line = node->line;

    xmlNode *child;

    child = node->children;
    while ( child ) {
        ExchangeProperty *property;
        if ( !parse_property(child, EXCHANGE_TYPE_KIND_CONST, &property, NULL) ) {
            exchange_key_free(*key);
            return FALSE;
        }
        if ( property ) {
            if ( str_iqual(property->name, ELEM_1C_GUID) ) {
                if ( property->type != EXCHANGE_TYPE_CONST_STRING || property->empty ) {
                    MY_ERROR("Строка %d. Неправильный тип или значение \"%s\". Ожидалась строка ", node->line, property->name);
                    exchange_property_free(property);
                    exchange_key_free(*key);
                    return FALSE;
                }
                (*key)->guid = g_value_dup_string(&property->value);
            }
            else
            if ( str_iqual(property->name, ELEM_1C_CODE) ) {
                if ( property->type != EXCHANGE_TYPE_CONST_STRING ) {
                    MY_ERROR("Строка %d. Неправильный тип или значение \"%s\". Ожидалась строка ", node->line, property->name);
                    exchange_property_free(property);
                    exchange_key_free(*key);
                    return FALSE;
                }
                if ( property->empty )
                    (*key)->code = g_strdup("");
                else
                    (*key)->code = g_value_dup_string(&property->value);
                g_strchomp((*key)->code);
            }
            else
            if ( str_iqual(property->name, ELEM_1C_CONST_NAME) ) {
                if ( property->type != EXCHANGE_TYPE_CONST_STRING || property->empty ) {
                    MY_ERROR("Строка %d. Неправильный тип или значение \"%s\". Ожидалась строка ", node->line, property->name);
                    exchange_property_free(property);
                    exchange_key_free(*key);
                    return FALSE;
                }
                (*key)->name = g_value_dup_string(&property->value);
            }
            else
            if ( str_iqual(property->name, ELEM_1C_GROUP) ) {
                if ( property->type != EXCHANGE_TYPE_CONST_BOOLEAN || property->empty  ) {
                    MY_ERROR("Строка %d. Неправильный тип или значение \"%s\". Ожидалось логическое значение ", node->line, property->name);
                    exchange_property_free(property);
                    exchange_key_free(*key);
                    return FALSE;
                }
                (*key)->group = g_value_get_boolean(&property->value);
            }
            else
            if ( str_iqual(property->name, ELEM_1C_OWNER) ) {
                if ( property->kind != EXCHANGE_TYPE_KIND_HANDBOOK ) {
                    MY_ERROR("Строка %d. Неправильный тип или значение \"%s\". Ожидалось определение ссылки ", node->line, property->name);
                    exchange_property_free(property);
                    exchange_key_free(*key);
                    return FALSE;
                }
                (*key)->owner_type = property->type;
                if ( !property->empty ) {
                    (*key)->owner = g_value_get_pointer(&property->value);
                }
            }
            exchange_property_free(property);
        }
        child = child->next;
    }
    if ( !(*key)->guid ) {
        exchange_key_free(*key);
        MY_ERROR("Строка %d. Неопределен \"%s\"", node->line, ELEM_1C_GUID);
        return FALSE;
    }
    if ( !(*key)->code && kind == EXCHANGE_TYPE_KIND_HANDBOOK ) {
        exchange_key_free(*key);
        MY_ERROR("Строка %d. Неопределен \"%s\"", node->line, ELEM_1C_CODE);
        return FALSE;
    }
    /*if ( !(*key)->name && kind == EXCHANGE_TYPE_KIND_ACCOUNT ) {
        exchange_key_free(*key);
        MY_ERROR("Строка %d. Неопределен \"%s\"", node->line, ELEM_1C_CONST_NAME);
        return FALSE;
    }*/
    return TRUE;
}

gboolean parse_value(xmlNode *node, ExchangeTypeKind kind, ExchangeProperty *property)
{
    if ( !node )
        return FALSE;

    if ( node->type != XML_ELEMENT_NODE ) {
        if ( node->line == 56399)
            return parse_value(node->next, kind, property);
        else
            return parse_value(node->next, kind, property);
    }

    if ( str_iqual((char*)node->name, ELEM_1C_EMPTY) ) {
        property->empty = TRUE;
        return TRUE;
    }

    if ( str_iqual((char*)node->name, ELEM_1C_LINK) ) {
        ExchangeKey *key;

        if ( !parse_key(node, kind, &key) )
            return FALSE;

        g_value_init(&property->value, G_TYPE_POINTER);
        g_value_set_pointer(&property->value, key);
        return TRUE;
    }

    if ( !str_iqual((char*)node->name, ELEM_1C_VALUE) ) {
        MY_ERROR("Строка %d. Неправильный элемент \"%s\". Ожидалось \"%s\"", node->line, node->name, ELEM_1C_VALUE);
        return FALSE;
    }
    gchar *text;
    gboolean res = TRUE;

    text = get_text(node);

    switch ( property->kind ) {
        case EXCHANGE_TYPE_KIND_CONST: {
            switch ( property->type ) {
                case EXCHANGE_TYPE_CONST_GUID:
                case EXCHANGE_TYPE_CONST_STRING:
                    g_value_init(&property->value, G_TYPE_STRING);
                    g_value_set_string(&property->value, g_strdup(text));
                    break;
                case EXCHANGE_TYPE_CONST_BOOLEAN:
                    g_value_init(&property->value, G_TYPE_BOOLEAN);
                    g_value_set_boolean(&property->value, str_iqual(text, CONST_1C_TRUE));
                    break;
                case EXCHANGE_TYPE_CONST_DATE:
                    g_value_init(&property->value, G_TYPE_STRING);
                    g_value_set_string(&property->value, g_strdup(text));
                    break;
                case EXCHANGE_TYPE_CONST_NUMBER:
                    g_value_init(&property->value, G_TYPE_DOUBLE);
                    g_value_set_double(&property->value, do_atof(text));
                    break;

                default:
                    MY_ERROR("Строка %d. У \"%s\" неописанный тип \"%s\"", node->line, node->name, exchange_type_name[property->type]);
                    res = FALSE;
            }
            break;
        }
        case EXCHANGE_TYPE_KIND_LIST:
            g_value_init(&property->value, G_TYPE_STRING);
            g_value_set_string(&property->value, g_strdup(text));
            break;
        default:
            MY_ERROR("Строка %d. У \"%s\" неописанный вид для типа \"%s\"", node->line, node->name, exchange_type_name[property->type]);
            res = FALSE;
    }

    g_free(text);
    return res;
}
ExchangeObject *find_exchange_object_by_key(GSList *objs, ExchangeKey *key)
{
    return find_exchange_object_by_guid(objs, key->guid);
}
GSList *find_exchange_objects_by_owner(GSList *objs, ExchangeType type, ExchangeObject *owner)
{
    GSList *l;
    GSList *res = NULL;
    for ( l = objs; l; l = l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( obj->type == type ) {
            if ( obj->key->owner &&
                 obj->key->owner_type == owner->type &&
                 !g_strcmp0(obj->key->owner->guid, owner->key->guid) ) {
                res = g_slist_append(res, obj);
            }
        }
    }
    return res;

}

GSList *find_exchange_objects_by_code(GSList *objs, ExchangeType type, const gchar *code)
{
    GSList *l, *res=NULL;
    for ( l = objs; l; l = l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( obj->type == type ) {
            if (!g_strcmp0(obj->key->code, code) ) {
                res = g_slist_append(res, obj);
            }
        }
    }
    return res;
}
GSList *find_exchange_objects_by_name(GSList *objs, ExchangeType type, const gchar *name)
{
    GSList *l, *res=NULL;
    for ( l = objs; l; l = l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( obj->type == type ) {
            ExchangeProperty *prop;
            prop = exchange_object_get_property(obj, ELEM_1C_NAME);
            if ( prop && !prop->empty && prop->type == EXCHANGE_TYPE_CONST_STRING ) {
                if (!g_strcmp0(g_value_get_string(&prop->value), name) ) {
                    res = g_slist_append(res, obj);
                }
            }
        }
    }
    return res;
}
GSList *find_exchange_objects_by_name_i(GSList *objs, ExchangeType type, const gchar *name)
{
    GSList *l, *res=NULL;
    gchar *name_up,*val_up;
    name_up = g_utf8_strup(name,-1);
    for ( l = objs; l; l = l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( obj->type == type ) {
            ExchangeProperty *prop;
            prop = exchange_object_get_property(obj, ELEM_1C_NAME);
            if ( prop && !prop->empty && prop->type == EXCHANGE_TYPE_CONST_STRING ) {
                val_up = g_utf8_strup(g_value_get_string(&prop->value),-1);
                if (!g_strcmp0(val_up, name_up) ) {
                    res = g_slist_append(res, obj);
                }
                g_free(val_up);
            }
        }
    }
    g_free(name_up)
;    return res;
}
ExchangeObject   *find_exchange_object_by_value_string(GSList *objs, ExchangeType type, const gchar *name, const gchar *str)
{
    GSList *l;
    for ( l = objs; l; l = l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( obj->type == type ) {
            ExchangeProperty *prop;
            prop = exchange_object_get_property(obj, name);

            if ( prop && !prop->empty ) {
                ExchangeTypeKind kind;
                kind = exchange_type_get_kind(prop->type);
                if ( prop->type == EXCHANGE_TYPE_CONST_STRING ||
                     kind == EXCHANGE_TYPE_KIND_LIST ) {
                    if ( !g_strcmp0(str, g_value_get_string(&prop->value)) )
                        return obj;
                }
            }
        }
    }
    return NULL;
}

ExchangeObject *find_exchange_object_by_const_name(GSList *objs, ExchangeType type, const gchar *code)
{
    GSList *l;
    for ( l = objs; l; l = l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( obj->type == type && !g_strcmp0(obj->key->name, code) )
            return obj;
    }
    return NULL;
}

/*
ExchangeObject   *find_exchange_list_by_value(GSList *objs, ExchangeType type, const gchar *value)
{
    if ( exchange_type_get_kind(type) != EXCHANGE_TYPE_KIND_LIST )
        return FALSE;
    GSList *l;
    for ( l = objs; l; l = l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( obj->type == type &&
             !obj->value->empty &&
             !g_strcmp0(g_value_get_string(&obj->value), value) )
            return obj;
    }
    return NULL;

}
*/

ExchangeObject *find_exchange_object_by_guid(GSList *objs, const gchar *guid)
{
    GSList *l;

    for ( l = objs; l; l=l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        if ( !g_strcmp0(obj->key->guid, guid) )
            return obj;
    }
    return NULL;
}
const gchar *exchange_object_get_name(ExchangeObject *obj)
{
    ExchangeProperty *name_prop;
    name_prop = exchange_object_get_property(obj, ELEM_1C_NAME);
    if ( !name_prop || name_prop->type != EXCHANGE_TYPE_CONST_STRING || name_prop->empty ) {
        OBJ_MY_ERROR(obj, "Неопределен или неправильный тип \"%s\"", ELEM_1C_NAME);
        return "";
    }
    return g_value_get_string(&name_prop->value);
}

ExchangeProperty *exchange_object_get_property(ExchangeObject *obj, const gchar *name)
{
    GSList *l;
    for ( l = obj->properties; l; l=l->next ) {
        ExchangeProperty *p;
        p = l->data;
        if ( str_iqual(p->name, name) )
            return p;
    }
    return NULL;
};
void obj_error(ExchangeObject *obj, const gchar *msg)
{
    MY_ERROR("Строка %d. %s. Объект \"%s\"", obj->line, msg, exchange_type_name[obj->type]);
}
void prop_error(ExchangeProperty *prop, const gchar *msg)
{
    MY_ERROR("Строка %d. %s. Параметр \"%s\"", prop->line, msg, prop->name);
}
void key_error(ExchangeKey *key, const gchar *msg)
{
    MY_ERROR("Строка %d. %s.", key->line, msg)
}

const gchar *get_partner_default_account(GSList *objs, ExchangeObject *partner, gboolean *many)
{
    ExchangeProperty *prop;
    ExchangeObject *account = NULL;
    *many = FALSE;

    prop = exchange_object_get_property(partner, ELEM_1C_DEFAULT_ACCOUNT);
    if ( !prop ) {
        OBJ_MY_ERROR(partner, "У контрагента \"%s\" не определен счет но умолчанию", exchange_object_get_name(partner));
        return NULL;
    }
    if ( !prop->empty ) {
        if ( prop->type != EXCHANGE_TYPE_HANDBOOK_ACCOUNT ) {
            PROP_MY_ERROR(prop, "Не правильный тип счета. Логическая ошибка");
            return NULL;
        }
        ExchangeKey *key;
        key = g_value_get_pointer(&prop->value);
        account = find_exchange_object_by_key(objs, key);
        if ( !account ) {
            PROP_MY_ERROR(prop, "Не найден объект с %s %s", ELEM_1C_GUID, key->guid);
            return NULL;
        }
    }
    if ( !account ) {
        GSList *accounts;
        accounts = find_exchange_objects_by_owner(objs, EXCHANGE_TYPE_HANDBOOK_ACCOUNT, partner);
        *many = g_slist_length(accounts) > 1;
        if ( *many )
            return NULL;
        account = accounts->data;
    }
    ExchangeProperty *a_prop;

    a_prop = exchange_object_get_property(account, ELEM_1C_ACCOUNT_NUMBER);
    if ( !a_prop || a_prop->type != EXCHANGE_TYPE_CONST_STRING ) {
        OBJ_MY_ERROR(account, "Неопределен или неправильный тип \"%s\"", ELEM_1C_ACCOUNT_NUMBER);
        return NULL;
    }
    if ( !a_prop->empty )
        return g_value_get_string(&a_prop->value);
    else {
        OBJ_MY_ERROR(account, "Счет пуст");
        return NULL;
    }
}
