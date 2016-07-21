<?php


/**
 * Class EfNodeMigration
 *
 * Common mappings for the Drupal 7 node migrations.
 */
abstract class EfNodeMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_legacy_nid', 'nid')
      ->description('We have a common field to save the D7 nid');
  }
}


/**
 * Class FestivalsMigration
 *
 * Migration class for Festivals node.
 */
class FestivalsMigration extends EfNodeMigration {

  public function __construct(array $arguments) {
    parent::__construct($arguments);

    // Remove  mapping to suppress warning.
    $this->removeFieldMapping('uid');
    $this->removeFieldMapping('body:language');

    $this->addSimpleMappings(array(
      'field_add_thisf',
      'field_age',
      'field_confirmed',
      'field_date',
      'field_facebook_event',
      'field_facebook_event:title',
      'field_facebook_event:attributes',
      'field_facebook_event:language',
      'field_facebook_promoter',
      'field_facebook_promoter:title',
      'field_facebook_promoter:attributes',
      'field_facebook_promoter:language',
      'field_festicket_logo',
      'field_festicket_packages',
      'field_festicket_shop',
      'field_festicket_shop:format',
      'field_festicket_url',
      'field_festival_classification',
      'field_gemischt',
      'field_genre_tags',
      'field_location',
      'field_visitors',
      'field_indoor',
      'field_type_of_event',
      'field_ticket_price',
      'field_youtube_event',
      'field_youtube_event:title',
      'field_youtube_event:attributes',
      'field_youtube_event:language',
      'field_url_event',
      'field_url_event:title',
      'field_url_event:attributes',
      'field_url_event:language',
      'field_url_promoter',
      'field_url_promoter:title',
      'field_url_promoter:attributes',
      'field_url_promoter:language',
      'field_lineup',
      'field_lineup:format',
      'field_youtube_promoter',
      'field_youtube_promoter:title',
      'field_youtube_promoter:attributes',
      'field_youtube_promoter:language',
      'field_trailer',
      'field_trailer:display',
      'field_trailer:description',
      'field_lineup_tags',
      'uid',
    ));

    $this->addFieldMapping('field_date:to', 'field_date:value2');

    $this->addFieldMapping('field_festicket_packages:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('field_festicket_packages:create_term')
      ->defaultValue(FALSE);
    $this->addFieldMapping('field_festicket_packages:ignore_case')
      ->defaultValue(FALSE);

    $this->addFieldMapping('field_genre_tags:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_genre_tags:create_term')->defaultValue(FALSE);
    $this->addFieldMapping('field_genre_tags:ignore_case')->defaultValue(FALSE);

    $this->addFieldMapping('field_lineup_tags:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('field_lineup_tags:create_term')
      ->defaultValue(FALSE);
    $this->addFieldMapping('field_lineup_tags:ignore_case')
      ->defaultValue(FALSE);

    $this->addFieldMapping('field_location:name', 'name');
    $this->addFieldMapping('field_location:street', 'street');
    $this->addFieldMapping('field_location:additional', 'additional');
    $this->addFieldMapping('field_location:city', 'city');
    $this->addFieldMapping('field_location:province', 'province');
    $this->addFieldMapping('field_location:postal_code', 'postal_code');
    $this->addFieldMapping('field_location:country', 'country');
    $this->addFieldMapping('field_location:latitude', 'latitude');
    $this->addFieldMapping('field_location:longitude', 'longitude');
    $this->addFieldMapping('field_location:source', 'source');
    $this->addFieldMapping('field_location:is_primary', 'is_primary');
    // In this case, we need to specify the file_class in the second paramter -
    // this class understands how to translate a http://www.youtube.com/ URI
    // into Drupal's Youtube file scheme (youtube://).
    $this->addFieldMapping('field_trailer:file_class')
      ->defaultValue('MigrateExtrasFileYoutube');


    $this->addUnmigratedSources(array(
      'body:language',
      'field_location:language',
      'field_date:language',
      'field_visitors:language',
      'field_indoor:language',
      'field_ticket_price:language',
      'field_type_of_event:language',
      'field_age:language',
      'field_add_thisf:language',
      'field_confirmed:language',
      'field_festicket_shop:language',
      'field_festival_classification:language',
      'field_lineup_tags:language',
      'field_genre_tags:language',
      'field_gemischt:language',
      'field_trailer:language',
      'field_lineup:language',
      'field_age:language',
      'field_add_thisf:language',
      'field_confirmed:language',
      'field_festicket_shop:language',
      'field_festival_classification:language',
      'field_lineup_tags:language',
      'field_genre_tags:language',
      'field_gemischt:language',
      'field_trailer:language',
      'field_lineup:language',
      'field_visitors:language',
      'field_indoor:language',
      'field_ticket_price:language',
      'field_type_of_event:language',
    ));

    $this->addUnmigratedDestinations(array(
      'field_date:rrule',
      'field_date:timezone',
      'field_festicket_url:title',
      'field_festicket_url:attributes',
      'field_festicket_url:language',
      'field_preview_img',
      'field_preview_img:file_class',
      'field_preview_img:preserve_files',
      'field_preview_img:destination_dir',
      'field_preview_img:destination_file',
      'field_preview_img:file_replace',
      'field_preview_img:source_dir',
      'field_preview_img:urlencode',
      'field_preview_img:alt',
      'field_preview_img:title',
    ));
  }

  public function prepareRow($row) {
    parent::prepareRow($row);

    if ($row->nid == 1100) {
      dsm($row);
      exit;
    }

    if (isset($row->field_genre_tags) && !empty($row->field_genre_tags)) {
      foreach ($row->field_genre_tags as $k => $genre) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'taxonomy_term')
          ->entityCondition('bundle', 'music_genre')
          ->fieldCondition('field_legacy_tid', 'value', $genre);
        $result = $query->execute();
        if (isset($result['taxonomy_term'])) {
          $row->field_genre_tags[$k] = array_shift(array_keys($result['taxonomy_term']));
        }
      }
    }

    if (isset($row->field_lineup_tags) && !empty($row->field_lineup_tags)) {
      foreach ($row->field_lineup_tags as $k => $lineup_tag) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'taxonomy_term')
          ->entityCondition('bundle', 'dj')
          ->fieldCondition('field_legacy_tid', 'value', $lineup_tag);
        $result = $query->execute();
        if (isset($result['taxonomy_term'])) {
          $row->field_lineup_tags[$k] = array_shift(array_keys($result['taxonomy_term']));
        }
      }
    }

    if (isset($row->field_festicket_packages) && !empty($row->field_festicket_packages)) {
      foreach ($row->field_festicket_packages as $k => $festicket_package) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'taxonomy_term')
          ->entityCondition('bundle', 'festicket_packages')
          ->fieldCondition('field_legacy_tid', 'value', $festicket_package);
        $result = $query->execute();
        if (isset($result['taxonomy_term'])) {
          $row->field_festicket_packages[$k] = array_shift(array_keys($result['taxonomy_term']));
        }
      }
    }
  }

  protected function query() {
    $query = parent::query();
    // Join location table, which is not CCK
    $query->leftJoin('location_instance', 'i', 'i.nid = n.nid AND i.vid = n.vid');
    $query->leftJoin('location', 'l', 'l.lid = i.lid');
    $query->fields('l', array(
      'name',
      'street',
      'additional',
      'city',
      'province',
      'postal_code',
      'country',
      'latitude',
      'longitude',
      'source',
      'is_primary',
    ));
    // $query->leftJoin('f', 'file_managed', '');
    // $quer
    return $query; // TODO: Change the autogenerated stub
  }
}

/**
 * Class OldLineupsMigration
 *
 * Migration for Oldlineups nodes.
 */
class OldLineupsMigration extends EfNodeMigration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.
    $this->sourceFields['lead_graphic'] = 'Lead graphic string from D6';
    $this->sourceFields['summary'] = 'Summary string from D6';

    parent::__construct($arguments);

    $this->addFieldMapping('field_image', 'field_lead_graphic')
      ->sourceMigration('Image');
    // Note that we map migrated terms by the vocabulary ID.
    $this->addFieldMapping('field_countries', '1')
      ->sourceMigration('Countries');
    $this->addFieldMapping('field_countries:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('field_topics', '2')
      ->sourceMigration('Topics');
    $this->addFieldMapping('field_topics:source_type')
      ->defaultValue('tid');
  }
}