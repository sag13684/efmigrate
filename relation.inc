<?php


/**
 * @file
 * Base class for migrating nodes into Drupal.
 */

/**
 * Base class for all node migrations - handles commonalities across all
 * supported source Drupal versions.
 *
 * In addition to the arguments supported by DrupalMigration, the following
 * must be passed in the $arguments array:
 *
 * source_type - Drupal 6 content type machine name.
 * destination_type - Drupal 7 content type machine name (bundle).
 *
 * The following optional arguments may be passed:
 *
 * user_migration - Machine name of a user migration, used to establish
 *   dependencies and a sourceMigration for the uid mapping.
 * default_uid - Drupal 7 (destination) uid of the user account to use as
 *   the default.
 * default_language - Default language for the node and node body. Defaults
 *   to LANGUAGE_NONE.
 */
abstract class DrupalRelationMigration extends DrupalMigration {
  /**
   * The source and destination content types (bundles) we're dealing with.
   */
  protected $destinationType;

  /**
   * Default language to apply to the node and it's body field.
   *
   * @var string
   */
  protected $defaultLanguage = LANGUAGE_NONE;

  /**
   * @param array $arguments
   */
  public function __construct(array $arguments) {
    $this->destinationType = $arguments['destination_type'];
    $this->sourceType = $arguments['source_type'];
    if (!empty($arguments['user_migration'])) {
      $user_migration = $arguments['user_migration'];
      $this->dependencies[] = $user_migration;
    }
    if (!empty($arguments['default_language'])) {
      $this->defaultLanguage = $arguments['default_language'];
    }
    parent::__construct($arguments);
    $arguments_keys = array_keys($arguments);
    // Required arguments from d2d see https://www.drupal.org/node/1813498
    $d2d_required_arguments = array(
      'source_connection',
      'source_version',
    );
    $required_arguments = array(
      'source_type',
      'destination_type',
      'max_arity',
      'sm_endpoint_source_bundle',
      'sm_endpoint_target_bundle',
    );
    $required_arguments = array_merge($d2d_required_arguments, $required_arguments);
    // arguments must contains all elements dictated by required arguments.
    $missing = array();
    foreach ($required_arguments as $required_argument) {
      if (!in_array($required_argument, $arguments_keys)) {
        $missing[] = $required_argument;
      }
    }
    if (!empty($missing)) {
      $message = t('!machine_name has missing required arguments: !missing',
        array(
          '!missing' => implode(', ', $missing),
          '!machine_name' => $this->machineName,
        )
      );
      self::displayMessage($message, MigrationBase::MESSAGE_NOTICE);
    }
    // Document known core fields
    $this->sourceFields = array(
      'rid' => t('Unique relation id (entity id).'),
      'relation_type' => t('Relation type (see relation_type table).'),
      'uid' => t('The {users}.uid that owns this relation; initially, this is the user that created it.'),
      'created' => t('Created timestamp'),
      'changed' => t('Modified timestamp'),
      'status' => t('Published'),
      'arity' => t('The number rows in this relation. Cannot exceed max_arity, or be less than min_arity in relation_type table.'),
    );

    // Get fields introduced through (field api)/(cck).
    $this->sourceFields = $this->version->getSourceFields('relation', $this->sourceType);

    $this->destination = new MigrateDestinationRelation($this->destinationType);

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'rid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Unique relation id (entity id).',
          'alias' => 'r',
        ),
      ),
      MigrateDestinationRelation::getKeySchema()
    );

    if (!$this->newOnly) {
      $this->highwaterField = array(
        'name' => 'changed',
        'alias' => 'r',
        'type' => 'int',
      );
    }

    if (isset($arguments['default_uid'])) {
      $default_uid = $arguments['default_uid'];
    }
    else {
      $default_uid = 1;
    }
    if (isset($user_migration)) {
      $this->addFieldMapping('uid', 'uid')
        ->sourceMigration($user_migration)
        ->defaultValue($default_uid);
    }
    else {
      $this->addFieldMapping('uid')
        ->defaultValue($default_uid);
    }
    $this->addUnmigratedDestinations(array('path'));
    $dnm = t('DNM');
    $endpoints_dnm_metas = array(
      'entity_id',
      'r_index',
    );
    $dnm_mappings = array(
      // Handled through prepare
      'endpoints' => $endpoints_dnm_metas,
    );
    foreach ($dnm_mappings as $dnm_map => $dnm_metas) {
      $this->addFieldMapping($dnm_map)->issueGroup($dnm);
      foreach ($dnm_metas as $dnm_meta) {
        $this->addFieldMapping($dnm_map . ':' . $dnm_meta)->issueGroup($dnm);
      }
    }
  }
}


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
    $this->removeFieldMapping('rid', 'rid');
    $this->removeFieldMapping('vid', 'vid');

    $this->addSimpleMappings(array(
      'created',
      'changed',
      'uid',
      'endpoints',
      'endpoints:entity_id',
      'endpoints:r_index',
    ));
    $this->addFieldMapping('rid')->defaultValue(FALSE);
    // $this->addFieldMapping('endpoints:entity_id');
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addUnmigratedSources(array(
      //'rid',
      'vid',
      'endpoints:language',
      'arity',
      // 'rid',
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

  /**
   * Called after the query data is fetched - we'll use this to populate the
   * source row with the CCK fields.
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    $this->version->getSourceValues($row, $row->rid);
    if ($row->{'endpoints:entity_id'}) {
      foreach ($row->{'endpoints:entity_id'}  as $k => $entity_id) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'node');
        $query->fieldCondition('field_legacy_nid', 'value', $entity_id);
        $result = $query->execute();
        if (isset($result['node'])) {
          $nids = array_keys($result['node']);
          $row->{'endpoints:entity_id'}[$k] = array_shift($nids);
        }
      }
    }
    if (isset($row->{'endpoints:entity_id'}[0])) {
      $node = new stdClass();
      $node->nid = $row->{'endpoints:entity_id'}[0];
      $node->type = 'old_lineups';
      $node->field_festival[LANGUAGE_NONE][0]['value'] = $row->{'endpoints:entity_id'}[0];
      field_attach_update('node', $node);
    }
    return TRUE;
  }
}