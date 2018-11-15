#include "grafod.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

/** ===== ARESTA ===== */

struct Aresta {
  InfoG info;

  char *label_origem;
  char *label_destino;
};

static struct Aresta *create_aresta(InfoG info, char *label_origem, char *label_destino) {
  struct Aresta *this = calloc(1, sizeof(*this));

  this->info = info;

  this->label_origem = label_origem;

  this->label_destino = label_destino;

  return this;
}

static void destroy_aresta(void *_this) {
  struct Aresta *this = _this;
  free(this);
}

/** ===== VERTICE ===== */

struct Vertice {
  InfoG info;

  char *label;
  HashTable arestas;
};

static struct Vertice *create_vertice(InfoG info, char *label) {
  struct Vertice *this = calloc(1, sizeof(*this));

  this->info = info;

  this->label = calloc(strlen(label) + 1, sizeof(char));
  strcpy(this->label, label);

  this->arestas = HashTable_t.create(7);

  return this;
}

static void destroy_vertice(void *_this) {
  struct Vertice *this = _this;
  HashTable_t.destroy(this->arestas, destroy_aresta, 0);

  free(this->label);

  free(this);
}

/** ===== GRAFO DIRECIONADO ===== */

struct GrafoD {
  int num_vertices;

  Lista vertices;
  HashTable label_x_vertice;
};

static GrafoD __create_grafod() {
  struct GrafoD *this = calloc(1, sizeof(*this));

  this->num_vertices = 0;

  this->vertices = Lista_t.create();
  this->label_x_vertice = HashTable_t.create(199);
  
  return this;
}

static void __destroy_grafod(GrafoD _this) {
  struct GrafoD *this = (struct GrafoD *) _this;

  Lista_t.destruir(this->vertices, NULL);
  HashTable_t.destroy(this->label_x_vertice, destroy_vertice, false);

  free(this);
}

/* ===== FUNCOES ARESTA ===== */

static void __insert_aresta_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    return;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    return;
  }

  struct Vertice *vertice_origem  = HashTable_t.get(this->label_x_vertice, origem).valor;
  struct Vertice *vertice_destino = HashTable_t.get(this->label_x_vertice, destino).valor;

  struct Aresta *aresta = create_aresta(NULL, vertice_origem->label, vertice_destino->label);

  HashInfo info = {
    .chave = vertice_destino->label,
    .valor = aresta
  };

  HashTable_t.insert(vertice_origem->arestas, info);

}

static void __define_info_aresta_grafod(GrafoD _this, char *origem, char *destino, InfoG info) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    return;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    return;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem).valor;

  if (!HashTable_t.exists(vertice_origem->arestas, destino)) {
    return;
  }

  struct Aresta *aresta = HashTable_t.get(vertice_origem->arestas, destino).valor;

  aresta->info = info;
}

static InfoG __get_info_aresta_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    return NULL;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    return NULL;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem).valor;

  if (!HashTable_t.exists(vertice_origem->arestas, destino)) {
    return NULL;
  }

  struct Aresta *aresta = HashTable_t.get(vertice_origem->arestas, destino).valor;

  return aresta->info;
}

static void __remove_aresta_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    return;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    return;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem).valor;

  if (!HashTable_t.exists(vertice_origem->arestas, destino)) {
    return;
  }

  struct Aresta *aresta = HashTable_t.get(vertice_origem->arestas, destino).valor;

  HashTable_t.remove(vertice_origem->arestas, destino);

  destroy_aresta(aresta);
}

static bool __adjacente_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    return false;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    return false;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem).valor;

  return HashTable_t.exists(vertice_origem->arestas, destino);
}

/* ===== FUNCOES VERTICE ===== */

static void __insert_vertice_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  if (HashTable_t.exists(this->label_x_vertice, node)) {
    return;
  }

  struct Vertice *vertice = create_vertice(NULL, node);

  HashInfo info = {
    .chave = vertice->label,
    .valor = vertice,
  };

  Lista_t.insert(this->vertices, vertice);
  HashTable_t.insert(this->label_x_vertice, info);

  this->num_vertices++;
}

static void __define_info_vertice_grafod(GrafoD _this, char *node, InfoG info) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, node)) {
    return;
  }

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node).valor;

  vertice->info = info;
}

static InfoG __get_info_vertice_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, node)) {
    return NULL;
  }

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node).valor;

  return vertice->info;
}

static void __remove_vertice_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, node)) {
    return;
  }

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node).valor;

  // Checar se tem alguma ligacao saindo dele
  if (HashTable_t.length(vertice->arestas) != 0) {
    return;
  }

  // Checar se tem alguma ligacao chegando nele
  // Todo: 

  HashTable_t.remove(this->label_x_vertice, node);

  destroy_vertice(vertice);
}

static void __adicionar_lista(const void *_aresta, void *_lista) {
  const struct Aresta *aresta = (const struct Aresta *) _aresta;
  Lista lista = _lista;

  Lista_t.insert(lista, aresta->label_destino);
}

static Lista __adjacentes_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  Lista lista = Lista_t.create();

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node).valor;

  HashTable_t.map(vertice->arestas, lista, __adicionar_lista);

  return lista;
}

const struct GrafoD_t GrafoD_t = { //
  .create              = &__create_grafod,
  .insert_aresta       = &__insert_aresta_grafod,
  .define_info_aresta  = &__define_info_aresta_grafod,
  .get_info_aresta     = &__get_info_aresta_grafod,
  .remove_aresta       = &__remove_aresta_grafod,
  .adjacente           = &__adjacente_grafod,
  .insert_vertice      = &__insert_vertice_grafod,
  .define_info_vertice = &__define_info_vertice_grafod,
  .get_info_vertice    = &__get_info_vertice_grafod,
  .remove_vertice      = &__remove_vertice_grafod,
  .adjacentes          = &__adjacentes_grafod,
  .destroy             = &__destroy_grafod,
};