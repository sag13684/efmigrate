<?php

/**
 * @file
 *
 */
class FestivalsOnlyRelations extends DrupalRelationMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->source = new MigrateSourceSQL($this->query(), $this->sourceFields, NULL,
      $this->sourceOptions);
    $this->removeFieldMapping('uid', 'uid');
    $this->removeFieldMapping('endpoints', 'endpoints');
    $this->removeFieldMapping('endpoints:entity_id', 'endpoints:entity_id');
    $this->removeFieldMapping('endpoints:r_index', 'endpoints:r_index');

    $this->addSimpleMappings(array(
      'created',
      'changed',
      'uid',
      'endpoints',
     // 'endpoints:entity_id',
      'endpoints:r_index',
    ));
    $this->addFieldMapping('endpoints:entity_id');
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addUnmigratedSources(array(
      'rid',
      'vid',
      'endpoints:language',
      'arity',
      'rid',
      'relation_type'
    ));
  }

  public function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('relation', 'r')
      ->fields('r')
      ->condition('r.relation_type', $this->sourceType)
      ->orderBy($this->newOnly ? 'r.rid' : 'r.changed');
    return $query;
  }
}