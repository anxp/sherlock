<ul>
<?php foreach ($constructed_queries as $key => $value) { ?>
    <li><?php print $key; ?>
        <ul>
            <?php foreach ($value as $v) { ?>
              <li>
                <?php print $v; ?>
              </li>
            <?php } unset($v); ?>
        </ul>
    </li>
<?php } unset($key, $value); ?>
</ul>